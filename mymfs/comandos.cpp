#include "pch.h"
#include "comandos.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <windows.h>
#include <filesystem>

namespace fsys = std::experimental::filesystem;
using namespace std;

bool mymfsEstaConfigurado(string caminhoComando) {
	if (caminhoComando.empty())
		return false;
	return fsys::exists(caminhoComando + "/mymfs.config");
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
			string nomeDiretorio = extensaoArquivo + "-" + caminhoArquivoImport.substr(0, caminhoArquivoImport.find("."));
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
				string linhaConfig = nomeDiretorio + ";" + to_string(numArquivos) + "\n";
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
		cout << "Nao foi possivel importar o arquivo. Arquivo mymfs.config está cheio - Mymfs." << endl;
	}
}

string verificarArquivoExisteEmConfig(string caminhoComando, string nomeArquivo) {

	ifstream arqConfig(caminhoComando + "/mymfs.config");

	string extensaoArquivo = nomeArquivo.substr(nomeArquivo.find(".") + 1, (nomeArquivo.size() - nomeArquivo.find(";")));
	string nomeDiretorioBuscado = extensaoArquivo + "-" + nomeArquivo.substr(0, nomeArquivo.find("."));   //Obtem o nome do diretorio atraves do nome do arquivo
	string nomeDiretorioEncontrado;
	string qtdArquivosEncontrado;
	string linhaConfig;

	do {
		//Percorre o arquivo config até o final ou até encontrar o arquivo a ser exportado
		getline(arqConfig, linhaConfig);
		nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
		qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(";") + 1,
			(linhaConfig.size() - linhaConfig.find(";")));
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

			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
			qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(";") + 1,
				(linhaConfig.size() - linhaConfig.find(";")));

			if (!nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()) {

				int numArquivos = stoi(qtdArquivosEncontrado);
				if (fsys::exists(caminhoDiretorioExport)) { //Verifica se o arquivo a ser exportado existe
					cout << "O arquivo a ser exportado ja existe na pasta destino ("
						<< caminhoDiretorioExport << "), por favor indique outro destino." << endl; //EDITADO TALITA
				}
				else {
					//Caso exista, cria um arquivo no diretorio informado concatenando todos os arquivos de 500KB
					ofstream combined_file(caminhoDiretorioExport);
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
			nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
			while (!arqConfig.eof() && !nomeDiretorioEncontrado.empty()) {
				//Exibe nome do diretório/arquivo
				cout << nomeDiretorioEncontrado + ".txt" << endl;
				string linhaConfig;
				getline(arqConfig, linhaConfig);
				nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
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
