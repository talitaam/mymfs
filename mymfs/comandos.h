#ifndef COMANDOS.H
#include <string>

void config (std::string caminhoComando);
void importarArquivo(std::string caminhoComando, std::string caminhoArquivoImport);
void exportarArquivo(std::string caminhoComando, std::string caminhoArquivoExport, std::string caminhoDiretorioExport);
void listAll(std::string caminhoComando);
void remove(std::string caminhoComando, std::string nomeArquivo);
void removeAll(std::string caminhoComando);
void procuraPalavra(std::string caminhoComando, std::string palavra, std::string caminhoArquivoToRead);
void primeiras100Linhas(std::string caminhoComando, std::string caminhoArquivoToRead);
void ultimas100Linhas(std::string caminhoComando, std::string caminhoArquivoToRead);

#endif //COMANDOS.H
