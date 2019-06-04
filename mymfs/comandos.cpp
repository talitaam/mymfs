#include "pch.h"
#include "comandos.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <filesystem>

namespace fsys = std::filesystem;
using namespace std;

bool mymfsEstaConfigurado(string caminhoComando) {
	if (caminhoComando.empty())
		return false;
	return fsys::exists(caminhoComando + "/mymfs.config");
}

bool arquivoEstaVazio(std::ifstream& pFile)
{
	return pFile.peek() == std::ifstream::traits_type::eof();
}

void config(string caminhoComando) {
	string caminhoConfig = caminhoComando + "/mymfs.config";

	if (mymfsEstaConfigurado(caminhoComando)) {   //Verifica se o arquivo Config ja existe no caminho especificado
		cout << "O Mymfs ja esta configurado nesta unidade." << endl;
	}
	else {
		ofstream arquivoConfig(caminhoConfig);       //Cria o arquivo config, configurando o Mymfs na unidade especificada
		arquivoConfig.close();
		cout << "O Mymfs foi configurado nesta unidade com sucesso." << endl;
	}
}

void importarArquivo(string caminhoComando, string caminhoArquivoImport) {
	streampos begin, end, pos;
	ifstream arqConfigExiste(caminhoComando + "/mymfs.config"); // Verifica se o arquivo Config existe
	if (mymfsEstaConfigurado(caminhoComando)) {
		arqConfigExiste.seekg(0);              //Busca o fim do arquivo para verificar o tamanho do mesmo
		arqConfigExiste.seekg(0, ios::end);
		end = arqConfigExiste.tellg();
		arqConfigExiste.close();
	}
	else {
		cout << "O Mymfs nao esta configurado na unidade informada." << endl; //Caso nao exista, nao permite a importacao
		return;
	}
	if (end < 50000) {                          //Apenas permite a importação se o arquivo de config for menor que 50KB

		if (!fsys::exists(caminhoComando + "/files")) {
			fsys::create_directory(caminhoComando + "/files");
		}

		if (!caminhoArquivoImport.empty()) {
			ifstream infile(caminhoArquivoImport, ifstream::binary);
			begin = infile.tellg();
			infile.seekg(0, ios::end);
			end = infile.tellg();
			infile.seekg(0);
			long sizeMax = 512000;          //Define o tamanho maximo dos arquivos como 500KB

			int numArquivos = ceil((end - begin) / 512000.0); // Verifica quantos arquivos de 500 KB ou menos serão criados

			char* buffer;
			infile.seekg(0);

			//Obtem o nome do diretório a ser criado para o arquivo atraves do seu nome
			string extensaoArquivo = caminhoArquivoImport.substr(caminhoArquivoImport.find(".") + 1, (caminhoArquivoImport.size() - caminhoArquivoImport.find(";")));
			string nomeDiretorio = caminhoArquivoImport;

			const size_t last_slash_idx = nomeDiretorio.find_last_of("\\/");
			if (std::string::npos != last_slash_idx)
			{
				nomeDiretorio.erase(0, last_slash_idx + 1);
			}

			nomeDiretorio = extensaoArquivo + "-" + nomeDiretorio.substr(0, nomeDiretorio.find("."));
			

			string caminhoDiretorioString = caminhoComando + "/files/" + nomeDiretorio;

			int erro = fsys::create_directory(caminhoDiretorioString);   //Cria o diretório para o arquivo a ser importado. Caso ocorra algum erro ou o diretório ja exista, retorna zero

			if (erro != 0) {           //Caso nao ocorram erros, cria os arquivos de 500KB ou menos
				for (int i = 0; i < numArquivos; i++) {
					auto s = to_string(i);
					//cout << "\nArquivo dividido: " << caminhoDiretorioString + "/" + s + ".txt" << " \n";

					ofstream outfile(caminhoDiretorioString + "/" + s + ".txt", ofstream::binary);

					if (i != numArquivos - 1) {  //Efetua a divisao do arquivo de importação em arquivos de 500KB ou menos
						infile.seekg(pos);
						buffer = new char[sizeMax];
						infile.read(buffer, sizeMax);
						pos = infile.tellg();
						outfile.write(buffer, sizeMax);
						delete[] buffer;
					}
					else {
						infile.seekg(pos);
						int finalSize = (end - pos);
						buffer = new char[finalSize];
						infile.read(buffer, finalSize);
						pos = infile.tellg();
						outfile.write(buffer, finalSize);
						delete[] buffer;
					}
					outfile.close();
				}

				ofstream arqConfig(caminhoComando + "/mymfs.config", ios_base::app | ios_base::out);
				ifstream arqConfigVazio(caminhoComando + "/mymfs.config");
				string linhaConfig;
				if (arquivoEstaVazio(arqConfigVazio)) {
					linhaConfig = nomeDiretorio + " " + to_string(numArquivos);
				}
				else {
					linhaConfig = "\n" + nomeDiretorio + " " + to_string(numArquivos);
				}
				arqConfigVazio.close();
				arqConfig << linhaConfig; //Adiciona o arquivo importado no arquivo de configuração (nomeArquivo;quantidadeArquivos)
				arqConfig.close();
				cout << "O arquivo foi importado para o Mymfs com sucesso." << endl;
			}
			else {
				cout << "Ocorreu um erro! Um arquivo com esse nome ja existe no Mymfs." << endl;
			}

			infile.close();
		}
		else {
			cout << "Arquivo nao importado no Mymfs pois o caminho informado esta vazio ou ambiente"
				<< " ainda nao foi configurado." << endl;
		}
	}
	else {
		cout << "Nao foi possivel importar o arquivo. Arquivo mymfs.config esta cheio - Mymfs." << endl;
	}
}

string verificarArquivoExisteEmConfig(string caminhoComando, string nomeArquivo) {

	ifstream arqConfig(caminhoComando + "/mymfs.config");

	string extensaoArquivo = nomeArquivo.substr(nomeArquivo.find(".") + 1, (nomeArquivo.size() - nomeArquivo.find(".")));
	string nomeDiretorioBuscado = extensaoArquivo + "-" + nomeArquivo.substr(0, nomeArquivo.find("."));   //Obtem o nome do diretorio atraves do nome do arquivo
	string nomeDiretorioEncontrado;
	string qtdArquivosEncontrado;
	string linhaConfig;

	do {
		//Percorre o arquivo config até o final ou até encontrar o arquivo a ser exportado
		getline(arqConfig, linhaConfig);
		nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
		qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(" ") + 1,
			(linhaConfig.size() - linhaConfig.find(" ")));
	} while (strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) != 0 &&
		!arqConfig.eof() && !nomeDiretorioEncontrado.empty());

	if (strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) == 0 &&
		!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {
		return linhaConfig;
	}

	return "";
}

void exportarArquivo(string caminhoComando, string nomeArquivoExport, string caminhoDiretorioExport) {
	ifstream arqConfigExiste(caminhoComando + "/mymfs.config");

	//Verifica se o arquivo de configuração e se o arquivo a ser exportado existem
	if (!nomeArquivoExport.empty() && !caminhoDiretorioExport.empty() && mymfsEstaConfigurado(caminhoComando)) {
		string nomeDiretorioEncontrado;
		string qtdArquivosEncontrado;

		//Percorre o arquivoConfig para obter a linha de configuração do arquivo a ser exportado
		string linhaConfig = verificarArquivoExisteEmConfig(caminhoComando, nomeArquivoExport);


		//Verifica se encontrou o diretorio do arquivo a ser exportado
		if (!linhaConfig.empty()) {

			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
			qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(" ") + 1,
				(linhaConfig.size() - linhaConfig.find(" ")));

			if (!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {

				int numArquivos = stoi(qtdArquivosEncontrado);
				if (fsys::exists(caminhoDiretorioExport + "/" + nomeArquivoExport)) { //Verifica se o arquivo a ser exportado existe
					cout << "O arquivo a ser exportado ja existe na pasta destino ("
						<< caminhoDiretorioExport + "/" + nomeArquivoExport << "), por favor indique outro destino." << endl; //EDITADO TALITA
				}
				else {
					//Caso exista, cria um arquivo no diretorio informado concatenando todos os arquivos de 500KB
					ofstream combined_file(caminhoDiretorioExport + "/" + nomeArquivoExport);
					for (int i = 0; i < numArquivos; i++) {
						auto s = to_string(i);
						s = s + ".txt";
						//Percorre os arquivos de 0 a numArquivos concatenando-os no arquivo exportado
						ifstream srce_file(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + s);
						if (srce_file) {
							combined_file << srce_file.rdbuf();
						}
						else {
							cerr << "Ocorreu um erro. O arquivo nao pode ser aberto " << s << '\n';
						}
						srce_file.close();
					}
					combined_file.close();
					cout << "O arquivo foi exportado do Mymfs com sucesso." << endl;
				}
			}
			else {
				cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi exportado" << endl;
			}
		}
		else {
			cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi exportado" << endl;
		}
	}
	else {
		cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente"
			<< " ainda nao foi configurado." << endl;
	}
}

void listAll(string caminhoComando) {
	//Valida se o arquivo config existe no diretorio especificado
	ifstream arqConfig(caminhoComando + "/mymfs.config");
	if (!caminhoComando.empty() && mymfsEstaConfigurado(caminhoComando)) {
		//Caso exista, percorre o arquivo buscando os nomes dos diretorios/arquivos e listando-os
		string nomeDiretorioEncontrado = "x";
		string qtdArquivosEncontrado;
		string linhaConfig;
		getline(arqConfig, linhaConfig);
		if (linhaConfig.length() > 0) {
			//Caso existam registros no arquivo config, eles serão buscados e exibidos
			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
			string extensaoDiretorio = linhaConfig.substr(0, linhaConfig.find("-"));
			nomeDiretorioEncontrado = nomeDiretorioEncontrado.substr(nomeDiretorioEncontrado.find("-") + 1, (nomeDiretorioEncontrado.size() - nomeDiretorioEncontrado.find("-")));
			while (!arqConfig.eof() && !nomeDiretorioEncontrado.empty()) {
				//Exibe nome do diretório/arquivo
				cout << nomeDiretorioEncontrado + "." << extensaoDiretorio << endl;
				string linhaConfig;
				getline(arqConfig, linhaConfig);
				nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
				extensaoDiretorio = linhaConfig.substr(0, linhaConfig.find("-"));
				nomeDiretorioEncontrado = nomeDiretorioEncontrado.substr(nomeDiretorioEncontrado.find("-") + 1, (nomeDiretorioEncontrado.size() - nomeDiretorioEncontrado.find("-")));
				cout << nomeDiretorioEncontrado + "." << extensaoDiretorio << endl;

			}
		}
		else {
			cout << "Nao ha arquivos salvos pelo Mymfs!" << endl;
		}
	}
	else {
		cout << "Arquivos nao listados do Mymfs pois o caminho informado esta vazio ou ambiente "
			<< "ainda nao foi configurado." << endl;
	}
}

string converterLinhaConfigParaNomeArquivo(string linhaConfig) {
	string nomeArquivoEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
	string extensaoArquivoEncontrado = nomeArquivoEncontrado.substr(0, nomeArquivoEncontrado.find("-"));
	nomeArquivoEncontrado = nomeArquivoEncontrado.substr(nomeArquivoEncontrado.find("-") + 1, (nomeArquivoEncontrado.size() - nomeArquivoEncontrado.find("-")));
	return nomeArquivoEncontrado + "." + extensaoArquivoEncontrado;
}

void remove(string caminhoComando, string nomeArquivo) {
	ifstream arqConfig(caminhoComando + "/mymfs.config");

	if (nomeArquivo.empty()) {
		cout << "Deve-se informar o nome do arquivo";
		return;
	}

	if (mymfsEstaConfigurado(caminhoComando)) {
		string linhaConfig = verificarArquivoExisteEmConfig(caminhoComando, nomeArquivo);
		string linhaConfigNovo;
		string configNovo;

		if (!linhaConfig.empty()) {

			getline(arqConfig, linhaConfigNovo);

			string nomeArquivoEncontrado = converterLinhaConfigParaNomeArquivo(linhaConfigNovo);
			if (nomeArquivoEncontrado != nomeArquivo) {
				configNovo += linhaConfigNovo;
			}

			if (linhaConfigNovo.length() > 0) {
				//Caso existam registros no arquivo config, eles serão buscados e exibidos
				while (!arqConfig.eof() && !linhaConfigNovo.empty()) {
					getline(arqConfig, linhaConfigNovo);

					nomeArquivoEncontrado = converterLinhaConfigParaNomeArquivo(linhaConfigNovo);
					if (nomeArquivoEncontrado != nomeArquivo) {
						configNovo += "\n" + linhaConfigNovo;
					}
				}
			}
			arqConfig.close();

			ofstream arquivoConfig(caminhoComando + "/mymfs.config", std::ofstream::out | std::ofstream::trunc);
			arquivoConfig << configNovo;
			arquivoConfig.close();

			string diretorioArquivoRemover = linhaConfig.substr(0, linhaConfig.find(" "));
			fsys::remove_all(caminhoComando + "/files/" + diretorioArquivoRemover);

			cout << "O arquivo ( " + nomeArquivo + " ) foi removido com sucesso." << endl;
		}
		else {
			arqConfig.close();
			cout << "O arquivo informado nao esta gravado no Mymfs, portanto nao foi removido";
		}
	}
	else {
		arqConfig.close();
		cout << "Os arquivos nao foram removidos pois o Mymfs nao esta configurado no caminho informado" << endl;
	}
}


void removeAll(string caminhoComando) {
	ofstream arquivoConfig;
	if (mymfsEstaConfigurado(caminhoComando)) {
		if (fsys::exists(caminhoComando + "/files")) {
			fsys::remove_all(caminhoComando + "/files");
			arquivoConfig.open(caminhoComando + "/mymfs.config", std::ofstream::out | std::ofstream::trunc);
			arquivoConfig.close();
			cout << "Os arquivos do Mymfs foram removidos com sucesso." << endl;
			return;
		}
		else {
			cout << "O Mymfs nao possui arquivos gravados, portanto nao foram removidos.";
		}
	}
	else {
		cout << "Os arquivos nao foram removidos pois o Mymfs nao esta configurado no caminho informado" << endl;
	}
}


void procuraPalavra(string caminhoComando, string palavra, string caminhoArquivoToRead) {
	ifstream arqConfigExiste(caminhoComando + "/mymfs.config");

	//Verifica se o arquivo de configuração e se o arquivo a ser exportado existem
	if (!caminhoArquivoToRead.empty() && mymfsEstaConfigurado(caminhoComando)) {
		string nomeDiretorioEncontrado;
		string qtdArquivosEncontrado;

		//Percorre o arquivoConfig para obter a linha de configuração do arquivo a ser exportado
		string linhaConfig = verificarArquivoExisteEmConfig(caminhoComando, caminhoArquivoToRead);


		//Verifica se encontrou o diretorio do arquivo a ser exportado
		if (!linhaConfig.empty()) {

			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
			qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(" ") + 1,
				(linhaConfig.size() - linhaConfig.find(" ")));

			if (!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {

				int numArquivos = stoi(qtdArquivosEncontrado);
				int contaLinha = 0;
				string linha = "";
				string ultimaLinha = "";
				int i;
				for (i = 0; i < numArquivos; i++) {
					auto s = to_string(i);
					s = s + ".txt";
					//Percorre os arquivos de 0 a numArquivos concatenando-os no arquivo exportado
					ifstream arqPesquisa(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + s);
					if (arqPesquisa) {
						while (!arqPesquisa.eof()) {
							getline(arqPesquisa, linha);
							contaLinha++;
							if (linha.length() > 0) {
								if (ultimaLinha.length() > 0) {
									linha = ultimaLinha + linha;
									ultimaLinha = "";
								}
								//if (strstr(linhaConfig.c_str(), palavra.c_str()) != NULL)
								//	break;
								if (linha.find(palavra) != -1) {
									cout << "Encontrado " << contaLinha << '\n' << endl;
									i = numArquivos + 1;
									return;
								}
							}
						}
						if (arqPesquisa.eof()) {
							arqPesquisa.seekg(1, ios::end);
							if (arqPesquisa.get() != '\n') {
								ultimaLinha = linha;
								contaLinha--;
							}
						}
					}
					else {
						cerr << "Ocorreu um erro. O arquivo nao pode ser aberto " << s << '\n';
					}
					arqPesquisa.close();
				}
				if (i == numArquivos) {
					cout << "Não Encontrado" << endl;
				}
				else {
					cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi feita a busca" << endl;
				}
			}
			else {
				cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi feita a busca" << endl;
			}
		}
		else {
			cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente"
				<< " ainda nao foi configurado." << endl;
		}
	}
}

void primeiras100Linhas(string caminhoComando, string caminhoArquivoToRead) {

	ifstream arqConfigExiste(caminhoComando + "/mymfs.config");

	//Verifica se o arquivo de configuração e se o arquivo a ser exportado existem
	if (!caminhoArquivoToRead.empty() && mymfsEstaConfigurado(caminhoComando)) {
		string nomeDiretorioEncontrado;
		string qtdArquivosEncontrado;

		//Percorre o arquivoConfig para obter a linha de configuração do arquivo a ser exportado
		string linhaConfig = verificarArquivoExisteEmConfig(caminhoComando, caminhoArquivoToRead);


		//Verifica se encontrou o diretorio do arquivo a ser exportado
		if (!linhaConfig.empty()) {

			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
			qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(" ") + 1,
				(linhaConfig.size() - linhaConfig.find(" ")));

			if (!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {

				int numArquivos = stoi(qtdArquivosEncontrado);
				int contaLinha = 0;
				string linha = "";
				string ultimaLinha = "";
				for (int i = 0; i < numArquivos; i++) {
					auto s = to_string(i);
					s = s + ".txt";
					//Percorre os arquivos de 0 a numArquivos concatenando-os no arquivo exportado
					ifstream arqPesquisa(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + s);
					if (arqPesquisa) {
						int loop = 0;
						int condicao = 100 - contaLinha;
						getline(arqPesquisa, linha);
						while (loop < condicao && !arqPesquisa.eof()) {
							contaLinha++;
							loop++;
							if (ultimaLinha.length() > 0) {
								linha = ultimaLinha + linha;
								ultimaLinha = "";
								cout << linha << endl;
							}
							else if (!arqPesquisa.eof())
								cout << linha << endl;
							getline(arqPesquisa, linha);
						}
						if (arqPesquisa.eof()) {
							arqPesquisa.seekg(1, ios::end);
							if (i == numArquivos - 1) {
								cout << linha << endl;
							}
							else if (arqPesquisa.get() != '\n') {
								ultimaLinha = linha;
								contaLinha--;
							}
						}
						if (contaLinha == 100)
							i = numArquivos;
					}
					else {
						cerr << "Ocorreu um erro. O arquivo nao pode ser aberto " << s << '\n';
					}
					arqPesquisa.close();
				}
			}
			else {
				cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foram exibidas as 100 primeiras linhas" << endl;
			}
		}
		else {
			cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foram exibidas as 100 primeiras linhas" << endl;
		}
	}
	else {
		cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente"
			<< " ainda nao foi configurado." << endl;
	}

}

void ultimas100Linhas(string caminhoComando, string caminhoArquivoToRead) {
	ifstream arqConfigExiste(caminhoComando + "/mymfs.config");

	//Verifica se o arquivo de configuração e se o arquivo existem
	if (!caminhoArquivoToRead.empty() && mymfsEstaConfigurado(caminhoComando)) {
		string nomeDiretorioEncontrado;
		string qtdArquivosEncontrado;

		//Percorre o arquivoConfig para obter a linha de configuração do arquivo
		string linhaConfig = verificarArquivoExisteEmConfig(caminhoComando, caminhoArquivoToRead);


		//Verifica se encontrou o diretorio do arquivo
		if (!linhaConfig.empty()) {

			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(" "));
			qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(" ") + 1,
				(linhaConfig.size() - linhaConfig.find(" ")));

			if (!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {

				int numArquivos = stoi(qtdArquivosEncontrado);
				int contaLinha = 0;
				int linhasUltimoArquivo = 0;
				string linha = "";
				string ultimaLinha = "";
				for (int i = numArquivos - 1; i >= 0; i--) {
					linhasUltimoArquivo = 0;
					auto s = to_string(i);
					s = s + ".txt";
					ifstream arqPesquisa(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + s);
					if (arqPesquisa) {
						getline(arqPesquisa, linha);
						do {
							linhasUltimoArquivo++;
							getline(arqPesquisa, linha);
						} while (!arqPesquisa.eof());
					}
					arqPesquisa.close();
					if (linhasUltimoArquivo >= 100 && contaLinha == 0) {
						int comecaLeitura = linhasUltimoArquivo - 100;
						ifstream arqPesquisa(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + s);
						if (arqPesquisa) {
							while (!arqPesquisa.eof()) {
								getline(arqPesquisa, linha);
								if (comecaLeitura == 0)
									cout << linha << endl;
								else
									comecaLeitura--;
							}
							i = -2;
						}
						arqPesquisa.close();
					}
					else if ((linhasUltimoArquivo + contaLinha) >= 100) {
						int faltaCompletarContaLinha = 100 - contaLinha;
						int comecaLeitura = linhasUltimoArquivo - faltaCompletarContaLinha;
						for (int j = i; j < numArquivos; j++) {
							auto k = to_string(j);
							k = k + ".txt";
							ifstream arqPesquisa(caminhoComando + "/files/" + nomeDiretorioEncontrado + "/" + k);
							if (arqPesquisa) {
								getline(arqPesquisa, linha);
								do {
									if (comecaLeitura == 0) {
										if (ultimaLinha.length() > 0) {
											linha = ultimaLinha + linha;
											ultimaLinha = "";
											cout << linha << endl;
										}
										else
											cout << linha << endl;
									}
									else
										comecaLeitura--;
									getline(arqPesquisa, linha);
								} while (!arqPesquisa.eof());
								if (arqPesquisa.eof()) {
									arqPesquisa.seekg(1, ios::end);
									if (arqPesquisa.get() != '\n' && j < numArquivos - 1) {
										ultimaLinha = linha;
									}
								}
							}
							arqPesquisa.close();
						}
						i = -2;
					}
					else if ((linhasUltimoArquivo + contaLinha) < 100) {
						contaLinha += linhasUltimoArquivo;
					}
				}
			}
			else {
				cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi exportado" << endl;
			}

		}
		else {
			cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi exportado" << endl;
		}
	}
	else {
		cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente"
			<< " ainda nao foi configurado." << endl;
	}

}