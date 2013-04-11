/* 
 * This class contains generic utility functions for the siren.
 * Author: ricardo
 *
 * Created on February 12, 2013, 12:11 PM
 */

#ifndef SIRENUTILS_H
#define	SIRENUTILS_H

#include <string>
#include <vector>
#include <iterator>
#include <boost/filesystem.hpp>

#include "Lex.h"

using namespace std;
using namespace boost;

class sirenUtils {
private:

	sirenUtils(sirenUtils const& copy);
	sirenUtils& operator=(sirenUtils const& copy);

	/**
	 * Private constructor.
	 */
	sirenUtils();

public:

	/**
	 * Converts a token object to a string.
	 * @param token
	 * 			token
	 * @return
	 * 			string
	 */
	string convertTokenListToString(Token* token);

	/**
	 * Verify if the file exists.
	 * @param fileName
	 * 			File name.
	 * @return
	 * 			If the file exists or not.
	 */
	static bool fileExists(const string& fileName);

	/**
	 * Singleton function.
	 * @return
	 * 			instance
	 */
	static sirenUtils& getInstance();

	template<typename T> int getPosition(vector<T>& vector, T& t) {
		typename std::vector<T>::const_iterator match;
		match = find(vector.begin(), vector.end(), t);
		return (vector.end() == match) ? -1 : match - vector.begin();
	}

	template<typename T> int getPositionCopy(vector<T> vector, T t) {
		typename std::vector<T>::const_iterator match;
		match = find(vector.begin(), vector.end(), t);
		return (vector.end() == match) ? -1 : match - vector.begin();
	}

	inline bool isNull(const string & word) {
			return (word.compare("") == 0);
		}

	/**
	 * Transforms the string to upper case.
	 * @param word
	 * 			word.
	 * @return
	 * 			word upper cased.
	 */
	string toUpper(const string& word);



};

#endif	/* SIRENUTILS_H */

