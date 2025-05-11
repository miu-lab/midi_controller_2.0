#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <dirent.h>
#include <sys/stat.h>

// Structure pour stocker les problèmes détectés
struct IssueInfo {
    std::string filePath;
    int lineNumber;
    std::string description;
    std::string codeLine;
};

std::vector<IssueInfo> issues;

// Vérifier si un chemin est un répertoire
bool isDirectory(const std::string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        return (s.st_mode & S_IFDIR);
    }
    return false;
}

// Vérifie si une ligne contient un membre de classe non conforme
void checkClassMembers(const std::string& filePath, int lineNumber, const std::string& line, bool inClass) {
    if (!inClass) return;
    
    // Ignorer les commentaires
    if (line.find("//") != std::string::npos) return;
    
    // Regex pour détecter les membres de classe (évite les mots clés, méthodes et types)
    std::regex memberRegex("\\s+(\\w+)(\\s*=|;)");
    std::smatch matches;
    
    if (std::regex_search(line, matches, memberRegex)) {
        std::string memberName = matches[1];
        
        // Ignorer les mots-clés et types communs
        if (memberName == "public" || memberName == "private" || memberName == "protected"
            || memberName == "class" || memberName == "struct" || memberName == "enum"
            || memberName == "const" || memberName == "static" || memberName == "virtual"
            || memberName == "explicit" || memberName == "return" || memberName == "void"
            || memberName == "int" || memberName == "bool" || memberName == "char"
            || memberName == "float" || memberName == "double" || memberName == "uint8_t"
            || memberName == "uint16_t" || memberName == "uint32_t") {
            return;
        }
        
        // Vérifier si le nom de membre se termine par _
        if (memberName.length() > 0 && memberName.back() != '_') {
            IssueInfo issue;
            issue.filePath = filePath;
            issue.lineNumber = lineNumber;
            issue.description = "Member variable without underscore suffix: " + memberName;
            issue.codeLine = line;
            issues.push_back(issue);
        }
    }
}

// Vérifie les getters commençant par "get"
void checkGetters(const std::string& filePath, int lineNumber, const std::string& line) {
    // Ignorer les commentaires et les définitions d'interface
    if (line.find("//") != std::string::npos || line.find("virtual") != std::string::npos) return;
    
    // Regex pour détecter les getters (qui commencent par "get" suivi d'une majuscule)
    std::regex getterRegex("\\s+\\w+\\s+get([A-Z]\\w*)\\s*\\(");
    std::smatch matches;
    
    if (std::regex_search(line, matches, getterRegex)) {
        std::string getterName = matches[1];
        
        IssueInfo issue;
        issue.filePath = filePath;
        issue.lineNumber = lineNumber;
        issue.description = "Getter with 'get' prefix: get" + getterName + "()";
        issue.codeLine = line;
        issues.push_back(issue);
    }
}

// Vérifie si la classe a une documentation Doxygen
void checkClassDocumentation(const std::string& filePath, std::vector<std::string> fileLines, size_t classIndex) {
    // Vérifier s'il y a des commentaires Doxygen avant la déclaration de classe
    bool hasDocumentation = false;
    
    // Remonter jusqu'à 5 lignes au-dessus de la déclaration de classe
    int start = (classIndex > 5) ? classIndex - 5 : 0;
    
    for (size_t i = start; i < classIndex; ++i) {
        if (fileLines[i].find("/**") != std::string::npos || fileLines[i].find("///") != std::string::npos) {
            hasDocumentation = true;
            break;
        }
    }
    
    if (!hasDocumentation) {
        IssueInfo issue;
        issue.filePath = filePath;
        issue.lineNumber = classIndex + 1;
        issue.description = "Class without Doxygen documentation";
        issue.codeLine = fileLines[classIndex];
        issues.push_back(issue);
    }
}

// Analyse un seul fichier
void analyzeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filePath << std::endl;
        return;
    }
    
    std::string line;
    int lineNumber = 0;
    bool inClass = false;
    std::vector<std::string> fileLines;
    
    // Lire tout le fichier
    while (std::getline(file, line)) {
        fileLines.push_back(line);
        
        // Vérifier si nous entrons dans une classe
        if (line.find("class ") != std::string::npos) {
            inClass = true;
            checkClassDocumentation(filePath, fileLines, fileLines.size() - 1);
        }
        
        // Vérifier si nous sortons d'une classe
        if (inClass && line.find("};") != std::string::npos) {
            inClass = false;
        }
        
        // Vérifier les membres et getters
        checkClassMembers(filePath, fileLines.size(), line, inClass);
        checkGetters(filePath, fileLines.size(), line);
    }
    
    file.close();
}

// Parcourt récursivement un répertoire
void analyzeDirectory(const std::string& dirPath) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        
        // Ignorer "." et ".."
        if (name == "." || name == "..") {
            continue;
        }
        
        std::string path = dirPath + "/" + name;
        
        if (isDirectory(path)) {
            // Ignorer les répertoires de compilation et de dépendances
            if (name != ".pio" && name != ".vscode" && name != "lib" && name != ".git" && name != "test") {
                analyzeDirectory(path);
            }
        } else {
            // Analyser uniquement les fichiers C++ et headers
            std::string ext = name.substr(name.find_last_of(".") + 1);
            if (ext == "cpp" || ext == "hpp" || ext == "h") {
                analyzeFile(path);
            }
        }
    }
    
    closedir(dir);
}

// Génère un rapport au format Markdown
void generateReport(const std::string& outputPath) {
    std::ofstream report(outputPath);
    if (!report.is_open()) {
        std::cerr << "Cannot create report file: " << outputPath << std::endl;
        return;
    }
    
    report << "# Standards de Code - Rapport d'Analyse\n\n";
    report << "## Problèmes Détectés\n\n";
    
    // Résumé
    report << "### Résumé\n\n";
    report << "Total des problèmes détectés: " << issues.size() << "\n\n";
    
    // Problèmes par type
    std::map<std::string, int> issuesByType;
    for (const auto& issue : issues) {
        std::string type = issue.description.substr(0, issue.description.find(":"));
        issuesByType[type]++;
    }
    
    report << "### Répartition par type\n\n";
    for (const auto& [type, count] : issuesByType) {
        report << "- " << type << ": " << count << "\n";
    }
    report << "\n";
    
    // Problèmes par fichier
    std::map<std::string, std::vector<IssueInfo>> issuesByFile;
    for (const auto& issue : issues) {
        issuesByFile[issue.filePath].push_back(issue);
    }
    
    report << "### Détails par fichier\n\n";
    for (const auto& [file, fileIssues] : issuesByFile) {
        report << "#### " << file << " (" << fileIssues.size() << " problèmes)\n\n";
        
        for (const auto& issue : fileIssues) {
            report << "- Ligne " << issue.lineNumber << ": " << issue.description << "\n";
            report << "  ```cpp\n  " << issue.codeLine << "\n  ```\n";
        }
        report << "\n";
    }
    
    report.close();
}

int main(int argc, char* argv[]) {
    std::string sourceDir = ".";
    if (argc > 1) {
        sourceDir = argv[1];
    }
    
    std::cout << "Analyzing directory: " << sourceDir << std::endl;
    analyzeDirectory(sourceDir);
    
    std::string outputPath = "standardization_reports/standards_report.md";
    std::cout << "Generating report to: " << outputPath << std::endl;
    generateReport(outputPath);
    
    std::cout << "Analysis complete. Found " << issues.size() << " issues." << std::endl;
    return 0;
}
