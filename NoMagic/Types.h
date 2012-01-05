#pragma once

namespace NoMagic
{
	typedef std::basic_string<TCHAR> tstring;
	typedef std::iterator<std::input_iterator_tag, BYTE> byteIterator;
	typedef std::vector<BYTE>::iterator byteVectorIterator;

	class MagicException
	{
		std::string msg;
		int error;
	public:
		MagicException(std::string message, int errorCode = 0) : msg(message), error(errorCode) {}
		std::string& GetMessage() { return msg; }
		int GetError() { return error; }
	};

	#define W32_CALL(fun) if(FALSE == fun) throw MagicException(#fun, GetLastError())

	/**
	Ein String gef&uuml;llt mit unsigned chars anstelle von signed chars.
	*/
	class byteString : public std::basic_string<BYTE>
	{
	public:
		byteString(const char* str) : std::basic_string<BYTE>(str, str+strlen(str)+1) {}
		byteString(std::string& str) : std::basic_string<BYTE>(str.begin(), str.end()) {}
		byteString(const BYTE* str) : std::basic_string<BYTE>(str) {}
	};

	/**
	Erleichtert das Erstellen von pattern Masken
	*/
	class PatternMask
	{
		PatternMask() {}
		~PatternMask() {}
	public:
		/**
		Erstellt eine pattern Maske.
		\param mask Die Maske (Bsp. "xxx??xxx")
		\param required Der Buchstabe, der das Byte als ben&ouml;tigt flagged. (Bsp. 'x')
		\return Explain me!
		*/
		static std::vector<bool> Make(std::string const& mask, const char required)
		{
			std::vector<bool> retVector;

			std::for_each(mask.begin(), mask.end(), [&](char chr)
			{
				if(chr == required)
					retVector.push_back(true);
				else
					retVector.push_back(false);
			});

			return retVector;
		}
	};
}