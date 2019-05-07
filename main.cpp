#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <windows.h>

using namespace std;

void importarArquivo(string caminhoComando, string caminhoArquivoImport);

int main(int argc, char **argv)
{
    cout << argc << endl;

    if(argc >= 2){
        string caminhoComando = argv[1];
        string comando = argv[2];
        cout << comando << endl;

        cout << "Comando: " << comando << endl;
        if(comando == "config"){
            string caminhoConfig = caminhoComando + "\config.txt";
            cout << "Caminho do arquivo Config: " << caminhoConfig << endl;
            std::ofstream arquivoConfig( caminhoConfig );

            arquivoConfig.close();
        }

        if(comando == "import"){
            string caminhoArquivoImport = argv[3];
            importarArquivo(caminhoComando, caminhoArquivoImport);
        }
    }


    //Dar merge

    //merge
    /*std::ofstream combined_file( "combined_file.txt" ) ;
    for(int i=0; i<numArquivos; i++)
    {
        auto s = std::to_string(i);
        cout << "\nnum do arquivo do arquivo a sofrer merge: " << s + ".txt" << " \n";
        s = s+".txt";
        std::ifstream srce_file(s) ;
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


    //iniciando o merge criando um arquivo file
    std::ifstream infile2 ("combined_file.txt",std::ifstream::binary);
    begin = infile2.tellg();
    cout << "begin is: " << begin << " \n";
    infile2.seekg (0, ios::end);
    end = infile2.tellg();
    cout << "end is: " << end << " \n";
    //infile.close();
    infile2.seekg (0);
    infile2.close();

    cout << "size combined_file is: " << (end-begin) << " bytes.\n";

    infile.close();*/
    return 0;
}

void importarArquivo(string caminhoComando, string caminhoArquivoImport){
    streampos begin, end, pos;

    if(!caminhoArquivoImport.empty()){
        std::ifstream infile (caminhoArquivoImport, std::ifstream::binary);
        begin = infile.tellg();
        cout << "begin is: " << begin << " \n";
        infile.seekg (0, ios::end);
        end = infile.tellg();
        cout << "end is: " << end << " \n";
        long size = (end-begin);
        infile.seekg (0);
        long sizeMax = 512000;

        cout << "size do arquivo passado is: " << (end-begin) << " bytes.\n";

        int numArquivos = ceil((end-begin)/512000.0); // saber quantos arquivos de 500 KB ou menos serão criados
        cout << "num arquivos: " << numArquivos<< " arquivos. Considerando tamanho máx de 500 KB por arquivo. \n";

        //dividr arquivo passado em arquivos menores de no máx 500 KB
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
        }
        else {
            cout << "Ocorreu um erro! Um arquivo com esse nome ja existe no Mymfs." << endl;
        }

        infile.close();
    }
}
