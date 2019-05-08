#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <windows.h>

using namespace std;

void config (string caminhoComando);
void importarArquivo(string caminhoComando, string caminhoArquivoImport);
void exportarArquivo(string caminhoComando, string caminhoArquivoExport, string caminhoDiretorioExport);
void listAll(string caminhoComando);

int main(int argc, char **argv)
{
    cout << "Bem vindo ao Mymfs!" << endl;

    //Deve possuir no minimo 3 argumentos (nome do programa - passado automaticamente,
    //caminho da unidade X, comando a ser executado)
    if(argc >= 3)
    {
        string caminhoComando = argv[1]; //Caminho de onde o Mymfs deve ser executado
        string comando = argv[2];        //Comando do Mymfs que deve ser executado

        cout << "Comando solicitado: " << comando << endl;

        if(comando == "config")
        {
            config(caminhoComando);
        }

        else if(comando == "import")
        {
            string caminhoArquivoImport = argv[3];    //Caminho do arquivo a ser importado para o diretório especficiado

            importarArquivo(caminhoComando, caminhoArquivoImport);
        }

        else if(comando == "export")
        {
            string caminhoArquivoExport = argv[3];  //Caminho do arquivo a ser exportado para o diretório especficiado
            string caminhoDiretorioExport = argv[4];  //Caminho do diretório para onde o arquivo deve ser exportado

            exportarArquivo(caminhoComando, caminhoArquivoExport, caminhoDiretorioExport);
        }
        else if(comando == "listall")
        {
            listAll(caminhoComando);
        }
    }
    else
    {
        cout << "Por favor, informe os argumentos necessarios" << endl;
    }

    return 0;
}

void config (string caminhoComando){
    string caminhoConfig = caminhoComando + "/config.txt";
    cout << "Caminho do arquivo Config: " << caminhoConfig << endl;

    std::ifstream arquivoConfigExiste( caminhoConfig );

    if(arquivoConfigExiste.good())              //Verifica se o arquivo Config ja existe no caminho especificado
    {
        cout << "O Mymfs ja esta configurado nesta unidade." << endl;
        arquivoConfigExiste.close();
    }
    else
    {
        arquivoConfigExiste.close();
        std::ofstream arquivoConfig( caminhoConfig );       //Cria o arquivo config, configurando o Mymfs na unidade especificada
        arquivoConfig.close();
        cout << "O Mymfs foi configurado nesta unidade com sucesso." << endl;
    }
}

void importarArquivo(string caminhoComando, string caminhoArquivoImport)
{
    streampos begin, end, pos;
    std::ifstream arqConfigExiste (caminhoComando + "/config.txt");
    if(arqConfigExiste.good())
    {
        arqConfigExiste.seekg (0);
        arqConfigExiste.seekg (0, ios::end);
        end = arqConfigExiste.tellg();
        arqConfigExiste.close();
    }
    if(end<50000)
    {
        if(!caminhoArquivoImport.empty())
        {
            std::ifstream infile (caminhoArquivoImport, std::ifstream::binary);
            begin = infile.tellg();
            //cout << "Comeco eh: " << begin << " \n";
            infile.seekg (0, ios::end);
            end = infile.tellg();
            //cout << "Fim eh: " << end << " \n";
            infile.seekg (0);
            long sizeMax = 512000;

            cout << "size do arquivo passado is: " << (end-begin) << " bytes.\n";

            int numArquivos = ceil((end-begin)/512000.0); // saber quantos arquivos de 500 KB ou menos serão criados
            cout << "Quantidade de arquivos arquivos: " << numArquivos<< " arquivos. Considerando tamanho max de 500 KB por arquivo. \n";

            //dividr arquivo passado em arquivos menores de no máx 500 KB
            char* buffer;
            infile.seekg(0);

            string nomeDiretorio = caminhoArquivoImport.substr(0, caminhoArquivoImport.find("."));
            //cout << "Nome Diretorio: " + nomeDiretorio << endl;
            string caminhoDiretorioString = caminhoComando + "/" + nomeDiretorio;
            char * caminhoDiretorio = new char [caminhoDiretorioString.length()+1];
            strcpy(caminhoDiretorio, caminhoDiretorioString.c_str());

            cout << "Caminho diretorio: " << caminhoDiretorio << endl;

            int erro = CreateDirectory(caminhoDiretorio, NULL);

            if(erro != 0)
            {
                for(int i=0; i<numArquivos; i++)
                {
                    auto s = std::to_string(i);
                    cout << "\nArquivo dividido: " << caminhoDiretorioString + "/" + s + ".txt" << " \n";

                    std::ofstream outfile (caminhoDiretorioString + "/" + s + ".txt",std::ofstream::binary);

                    if(i!= numArquivos-1)
                    {
                        infile.seekg(pos);
                        //cout << "pos antes eh: " << pos << " \n";
                        buffer = new char[sizeMax];
                        infile.read (buffer,sizeMax);
                        pos = infile.tellg();
                        //cout << "pos antes eh: " << pos << " \n";
                        outfile.write (buffer,sizeMax);
                        delete[] buffer;
                    }
                    else
                    {
                        infile.seekg(pos);
                        //cout << "pos antes eh: " << pos << " \n";
                        int finalSize = (end-pos);
                        //cout << "tamanho final: " << finalSize << " \n";
                        buffer = new char[finalSize];
                        infile.read (buffer,finalSize);
                        pos = infile.tellg();
                        //cout << "pos depois eh: " << pos << " \n";
                        outfile.write (buffer,finalSize);
                        delete[] buffer;
                    }
                    outfile.close();
                }

                std::ofstream arqConfig (caminhoComando + "/config.txt", std::ios_base::app | std::ios_base::out);
                string linhaConfig = nomeDiretorio + ";" + std::to_string(numArquivos) + "\n";
                arqConfig << linhaConfig;
                arqConfig.close();
                cout << "O arquivo foi importado para o Mymfs com sucesso." << endl;
            }
            else
            {
                cout << "Ocorreu um erro! Um arquivo com esse nome ja existe no Mymfs." << endl;
            }

            infile.close();
        }
        else
        {
            cout << "Arquivo nao importado no Mymfs pois o caminho informado esta vazio ou ambiente ainda nao foi configurado." << endl;
        }
    }
    else
        cout << "Nao foi possivel importar o arquivo. Arquivo config.txt está cheio - Mymfs." << endl;
}

void exportarArquivo(string caminhoComando, string caminhoArquivoExport, string caminhoDiretorioExport)
{
//Dar merge
    std::ifstream arqConfigExiste(caminhoComando + "/config.txt");
    std::ifstream arqExportExiste(caminhoArquivoExport);

    if(arqConfigExiste.good() &&  arqExportExiste.good() && !caminhoArquivoExport.empty()&& !caminhoDiretorioExport.empty())
    {
        arqExportExiste.close();
        string nomeDiretorioBuscado = caminhoArquivoExport.substr(0, caminhoArquivoExport.find("."));
        string nomeDiretorioEncontrado;
        string qtdArquivosEncontrado;

        do
        {
            string linhaConfig;
            std::getline(arqConfigExiste, linhaConfig);
            nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
            qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(";") + 1, (linhaConfig.size()-linhaConfig.find(";")));
        }
        while (strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) != 0 &&
                !arqConfigExiste.eof() && !nomeDiretorioEncontrado.empty() ); //EDITADO TALITA

        if(strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) == 0 &&
                !nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty())
        {
            int numArquivos = stoi(qtdArquivosEncontrado);
            cout << "Arquivo encontrado! Exportando..." << endl;
            std::ifstream arquivoJaExiste(caminhoDiretorioExport); //EDITADO TALITA
            if(arquivoJaExiste.good())
            {
                arquivoJaExiste.close();
                cout << "O arquivo a ser exportado ja existe na pasta destino (" << caminhoDiretorioExport << "), por favor indique outro destino." << endl; //EDITADO TALITA
            }
            else
            {
                std::ofstream combined_file(caminhoDiretorioExport); //EDITADO TALITA
                for(int i=0; i<numArquivos; i++)
                {
                    auto s = std::to_string(i);
                    cout << "\nArquivo concatenado: " << s + ".txt" << " \n";
                    s = s+".txt";
                    std::ifstream srce_file(caminhoComando + "/" + nomeDiretorioEncontrado + "/" + s) ;
                    if(srce_file)
                    {
                        combined_file << srce_file.rdbuf() ;
                        if(combined_file)
                        {
                            std::cout << "Arquivo " + s + " concatenado\n" ;
                        }
                    }
                    else
                    {
                        std::cerr << "Ocorreu um erro. O arquivo nao pode ser aberto " << s << '\n' ;
                    }
                    srce_file.close();
                }
                combined_file.close();
                cout << "O arquivo foi exportado do Mymfs com sucesso." << endl;
            }

        }
        else
        {
            cout << "O arquivo nao foi encontrado no Mymfs, portanto nao foi exportado" << endl;
        }
    }
    else
    {
        arqExportExiste.close();
        cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente ainda nao foi configurado." << endl;
    }
}

void listAll(string caminhoComando)  //EDITADO TALITA
{
//Dar merge
    std::ifstream arqConfigExiste(caminhoComando + "/config.txt");
    if(arqConfigExiste.good())
    {
        string nomeDiretorioEncontrado = "x";
        string qtdArquivosEncontrado;
        string linhaConfig;
        std::getline(arqConfigExiste, linhaConfig);
        if(linhaConfig.length() > 0)
        {
            cout << "Arquivos encontrados: \n" << endl;
            nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
            while (!arqConfigExiste.eof() && !nomeDiretorioEncontrado.empty())
            {
                cout << nomeDiretorioEncontrado + ".txt" << endl;
                string linhaConfig;
                std::getline(arqConfigExiste, linhaConfig);
                nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
            }
            cout << "\nFim da lista de arquivos encontrados. \n" << endl;
        }
        else
        {
            cout << "Nao ha arquivos salvos pelo Mymfs!" << endl;
        }
    }
    else
    {
        cout << "Arquivos nao listados do Mymfs pois o caminho informado esta vazio ou ambiente ainda nao foi configurado." << endl;
    }
}
