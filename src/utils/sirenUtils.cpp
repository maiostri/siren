#include "sirenUtils.h"
#include "Lex.h"

sirenUtils::sirenUtils() {

}

string sirenUtils::convertTokenListToString(Token* token) {
	string tokenList;
	while (token->Prior != NULL) {
		token = token->Prior;
	}
	while (token->Next != NULL) {
		tokenList +=
				(((token->Next->Lexem).compare(".") == 0) || (token->Next->Lexem.compare(",") == 0)
						|| (token->Lexem).compare(".") == 0) ? token->Lexem : token->Lexem + " ";
		token = token->Next;
	}
	tokenList += token->Lexem;
	return tokenList;
}

bool sirenUtils::fileExists(const string& fileName) {
	return boost::filesystem::exists(fileName);
}

sirenUtils& sirenUtils::getInstance() {
	static sirenUtils instance;
	return instance;
}

string sirenUtils::toUpper(const string& word) {
	string result = word;
	transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}

