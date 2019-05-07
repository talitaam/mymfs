#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <windows.h>
#include <filesystem>

using namespace std;

void importarArquivo(string caminhoComando, string caminhoArquivoImport);
void exportarArquivo(string caminhoComando, string caminhoArquivoExport);

int main(int argc, char **argv)
{
    cout << argc << endl;

    if(argc >= 2){
        string caminhoComando = argv[1]; //Caminho de onde o Mymfs deve ser executado
        string comando = argv[2];        //Comando do Mymfs que deve ser executado
        cout << comando << endl;

        cout << "Comando: " << comando << endl;
        if(comando == "config"){
            string caminhoConfig = caminhoComando + "\\config.txt";
            cout << "Caminho do arquivo Config: " << caminhoConfig << endl;
            std::ofstream arquivoConfig( caminhoConfig );
            arquivoConfig.close();
        }

        else if(comando == "import"){
            string caminhoArquivoImport = argv[3];    //Caminho do arquivo a ser importado para o diret�rio especficiado
            importarArquivo(caminhoComando, caminhoArquivoImport);
        }

        else if(comando == "export"){
            string caminhoArquivoExport = argv[3];  //Caminho do arquivo a ser exportado para o diret�rio especficiado
            //TODO: obter caminho de onde o arquivo exportado deve ir
            exportarArquivo(caminhoComando, caminhoArquivoExport);
        }
    }
    else{
        cout << "Por favor, informe os argumentos necessarios" << endl;
    }



    return 0;
}

void importarArquivo(string caminhoComando, string caminhoArquivoImport){
    streampos begin, end, pos;
    std::ifstream arqConfigExiste (caminhoComando + "/config.txt");
    if(arqConfigExiste.good() && !caminhoArquivoImport.empty()){
        std::ifstream infile (caminhoArquivoImport, std::ifstream::binary);
        begin = infile.tellg();
        cout << "begin is: " << begin << " \n";
        infile.seekg (0, ios::end);
        end = infile.tellg();
        cout << "end is: " << end << " \n";
        infile.seekg (0);
        long sizeMax = 512000;

        cout << "size do arquivo passado is: " << (end-begin) << " bytes.\n";

        int numArquivos = ceil((end-begin)/512000.0); // saber quantos arquivos de 500 KB ou menos ser�o criados
        cout << "num arquivos: " << numArquivos<< " arquivos. Considerando tamanho m�x de 500 KB por arquivo. \n";

        //dividr arquivo passado em arquivos menores de no m�x 500 KB
        char* buffer;
        infile.seekg(0);

        string nomeDiretorio = caminhoArquivoImport.substr(0, caminhoArquivoImport.find("."));
        cout << "Nome Diretorio: " + nomeDiretorio << endl;
        string caminhoDiretorioString = caminhoComando + "/" + nomeDiretorio;
        char * caminhoDiretorio = new char [caminhoDiretorioString.length()+1];
        strcpy(caminhoDiretorio, caminhoDiretorioString.c_str());

        cout << "caminho diretorio: " << caminhoDiretorio << endl;

        int erro = CreateDirectory(caminhoDiretorio, NULL);

        if(erro != 0){
            for(int i=0; i<numArquivos; i++) {
                auto s = std::to_string(i);
                cout << "\nnum arquivo do arquivo dividido: " << caminhoComando + "/" + s + ".txt" << " \n";

                std::ofstream outfile (caminhoDiretorioString + "/" + s + ".txt",std::ofstream::binary);

                if(i!= numArquivos-1) {
                    infile.seekg(pos);
                    cout << "pos antes is: " << pos << " \n";
                    buffer = new char[sizeMax];
                    infile.read (buffer,sizeMax);
                    pos = infile.tellg();
                    cout << "pos antes is: " << pos << " \n";
                    outfile.write (buffer,sizeMax);
                    delete[] buffer;
                }
                else {
                    infile.seekg(pos);
                    cout << "pos antes is: " << pos << " \n";
                    int finalSize = (end-pos);
                    cout << "finalSize: " << finalSize << " \n";
                    buffer = new char[finalSize];
                    infile.read (buffer,finalSize);
                    pos = infile.tellg();
                    cout << "pos depois is: " << pos << " \n";
                    outfile.write (buffer,finalSize);
                    delete[] buffer;
                }
                outfile.close();
            }

            std::ofstream arqConfig (caminhoComando + "/config.txt", std::ios_base::app | std::ios_base::out);
            string linhaConfig = nomeDiretorio + ";" + std::to_string(numArquivos) + "\n";
            arqConfig << linhaConfig;
            arqConfig.close();

        }
        else {
            cout << "Ocorreu um erro! Um arquivo com esse nome ja existe no Mymfs." << endl;
        }

        infile.close();
    }
    else {
         cout << "Arquivo nao importado no Mymfs pois o caminho informado esta vazio ou ambiente ainda nao foi configurado." << endl;
    }
}

void exportarArquivo(string caminhoComando, string caminhoArquivoExport){
//Dar merge
    std::ifstream arqConfigExiste (caminhoComando + "/config.txt");
    if(arqConfigExiste.good() && !caminhoArquivoExport.empty()){
        string nomeDiretorioBuscado = caminhoArquivoExport.substr(0, caminhoArquivoExport.find("."));
        string nomeDiretorioEncontrado;
        string qtdArquivosEncontrado;
        do{

            string linhaConfig;
            std::getline(arqConfigExiste, linhaConfig);
            nomeDiretorioEncontrado = linhaConfig.substr(0, linhaConfig.find(";"));
            qtdArquivosEncontrado = linhaConfig.substr(linhaConfig.find(";") + 1, (linhaConfig.size()-linhaConfig.find(";")));
            cout << nomeDiretorioEncontrado << endl;
            cout <<"qtd: " + qtdArquivosEncontrado << endl;

        } while (strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) != 0 &&
                 !nomeDiretorioEncontrado.empty());

        if(strcmp(nomeDiretorioEncontrado.c_str(), nomeDiretorioBuscado.c_str()) == 0 &&
                 !nomeDiretorioEncontrado.empty() && !qtdArquivosEncontrado.empty()){
            int numArquivos = stoi(qtdArquivosEncontrado);

            //TODO: exportar para pasta do args[4] e nome do txt com nomeDiretorioEncontrado
            std::ofstream combined_file( "combined_file.txt" );
            for(int i=0; i<numArquivos; i++)
            {
                auto s = std::to_string(i);
                cout << "\nnum do arquivo do arquivo a sofrer merge: " << s + ".txt" << " \n";
                s = s+".txt";
                std::ifstream srce_file(caminhoComando + "/" + nomeDiretorioEncontrado + "/" + s) ;
                if(srce_file)
                {
                    combined_file << srce_file.rdbuf() ;
                    if(combined_file){
                        std::cout << "file " + s + " appended\n" ;
                    }
                }
                else
                {
                    std::cerr << "error: could not open " << s << '\n' ;
                }
                srce_file.close();
            }
            combined_file.close();
        }

    }
    else {
         cout << "Arquivo nao exportado do Mymfs pois o caminho informado esta vazio ou ambiente ainda nao foi configurado." << endl;
    }
}
