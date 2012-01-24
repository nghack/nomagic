/*  Copyright (C) 2012 Dennis Garske

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


namespace NoMagic
{
	class NoMagic
	{
	private:
		/// Die Prozess ID
		DWORD procID;
		///Handle zum Prozess
		HANDLE process;
		///Basisadresse der ge&ouml;ffneten .exe
		UINT_PTR baseAddress;
		///Die Gr&ouml;&szlig;e des in den Arbeitsspeicher geladenen Programmes
		UINT_PTR moduleSize;
		///Legt fest, ob wir uns im Prozess oder au&szlig;erhalb des Prozesses befinden
		bool bInProcess;

		///Lie&szlig;t die Basiadresse der .exe
		void GetBaseAddress(DWORD procID, const TCHAR* name);

		/**
		Lädt die in path angegebene .dll in den geöffneten Prozess.
		\param path Der Pfad zur .dll
		\param outLoadedModule Pointer zum geladenen Modul.
		\brief Wirft MagicException bei Fehlschlag
		*/
		void LoadDLLIntoProcess(tstring const& path, HMODULE& outLoadedModule) const;

		/**
		Findet die Adresse der Funktion "Start" im geladenen Modul
		\param loadedModule Das geladene Modul
		\param path Pfad zur .dll
		\param outStartAddress Die gefundene Adresse
		\brief Wirft MagicException bei Fehlschlag
		*/
		void FindStartFunctionAddress(HMODULE& loadedModule, tstring const& path, UINT_PTR& outStartAddress) const;

		/**
		Ruft die Funktion "Start" im fremden Prozess auf.
		\param startFunctionAddress Die Addresse der Start Funktion im fremden Prozess
		\brief Wirft MagicException bei Fehlschlag
		*/
		void CallStart(UINT_PTR startFunctionAddress) const;
	public:
		NoMagic(void);
		~NoMagic(void);
	
		/**
		&ouml;ffnet den ersten gefundenen Prozess mit dem Namen der .exe Datei.
		\param name R&uuml;ckgabewert: Prozess ID
		\return ProzessID des Prozesses.
		\brief Wirft MagicException bei Fehlschlag
		*/
		DWORD OpenProcess(tstring const& name);

		/**
		Gibt den HANDLE zum ge&ouml;ffneten Prozess zur&uuml;ck.
		\return Handle zum Prozess.
		*/
		HANDLE GetProcessHandle() const;

		/**
		Gibt die ID des Prozesses zurück.
		\return Prozess ID.
		*/
		DWORD GetProcessID() const;

		/**
		Gibt die Startadresse des Modules zurück.
		\return Startadresse des Modules.
		*/
		UINT_PTR GetBaseAddress() const;

		/**
		Gibt die letzte Adresse des Modules zurück.
		\return Die letzte Adresse des Modules
		*/
		UINT_PTR GetModuleSize() const;

		/**
		Sind wir im fremden Prozess?
		*/
		bool IsInProcess();

		/**
		Teilt der Library mit, ob sie im fremden Prozess benutzt wird oder nicht.
		\param isInProc Befinden wir uns im fremden Prozess oder nicht?
		*/
		void SetInProcess(bool isInProc);
		
		/**
		Sucht nach dem angegebenen Muster im Arbeitsspeicher und gibt, bei &uuml;bereinstimmung,
		die Adresse zur&uuml;ck, an der eine &uuml;bereinstimmung festgestellt wurde.
		
		\brief Instructions: http://sparksandflames.com/files/x86InstructionChart.html
		\brief Ist schneller, wenn bInProcess == true ist.
		\param pattern = Das gesuchte Muster (Bsp: "\x53\xA3\x00\x00\x00\x00\x52" -> push EBX;MOV [0000], EAX; push EDX;)
		\param mask = Die Suchmaske. (Bsp: "xx????x" -> die ersten zwei Bytes m&uuml;ssen &uuml;bereinstimmen. Die n&auml;chsten 4 
		m&uuml;ssen nur da sein und das letzte muss wieder &uuml;bereinstimmen).
		\param algorithm Der Algorithmus, der zur Suche verwendet werden soll (Standard: Boyer-Moore-Horspool O(n/m) )
		\return NULL, wenn nichts gefunden wurde, ansonsten Adresse.
		*/
		UINT_PTR SearchPattern(byteString const& pattern, std::vector<bool>& mask, Algorithm::IPatternAlgorithm& algorithm = Algorithm::BoyerMooreHorspool()) const;

		/**
		Gibt dem Thread Debugrechte, damit z.B. fremder Speicher bearbeitet werden kann.
		*/
		void SetDebugPrivileges() const;
	
		/**
		Injiziert eine .dll in den ge&ouml;ffneten Prozess.
		\param path Der Pfad zur .dll Datei.
		\return Adresse der geladenen Library
		*/
		UINT_PTR InjectDll(tstring const& path) const;

		/**
		Entlädt eine injizierte .dll aus den ge&ouml;ffneten Prozess.
		\param address Adresse der geladenen Library 
		*/
		void UnloadDll(UINT_PTR address) const;

		/**
		Schreibt eine "Umleitung" f&uuml;r eine Funktion, damit unsere Funktion aufgerufen wird.
		\param targetFunction Die Funktion, die wir umleiten m&ouml;chten.
		\param newFunction Die Funktion, die anstelle der urspr&uuml;nglichen Funktion ausgef&uumlhrt werden soll.
		\return Pointer zur Originalen Funktion.
		*/
		PBYTE HookFunction(PBYTE targetFunction, PBYTE newFunction) const;

		/**
		Entfernt eine Umleitung.
		\param origFunction Die urspr&uuml;ngliche Funktion, die umgelenkt wurde.
		\param yourFunction Unsere Funktion, auf die wir umgelenkt haben.
		\return TRUE bei Erfolg, FALSE bei Misserfolg.
		*/
		BOOL UnhookFunction(PBYTE origFunction, PBYTE yourFunction) const;

		/**
		Lie&szlig;t sizeof(type) Bytes aus dem Speicher. 
		\brief Strings sollten damit nicht gelesen werden!<br/>Wirft MagicException bei Fehlschlag
		\param address Die Adresse, an der wir auslesen m&ouml;chten.
		\param relative Wenn == true dann wird die angegebene Adresse zur Basis hinzugerechnet.
		\return ausgelesener Wert.
		*/
		template <typename type> type Read(UINT_PTR address, bool relative = false) const
		{
			type buffer;
			LPCVOID addr = reinterpret_cast<LPVOID>(relative ? address+baseAddress : address);
			W32_CALL(ReadProcessMemory(process, addr, &buffer, sizeof(buffer), nullptr));

			return buffer;
		}
		
		/**
		Lie&szlig;t 512 Bytes aus dem Speicher. 
		\param address Die Adresse, an der wir auslesen m&ouml;chten.
		\param relative Wenn == true dann wird die angegebene Adresse zur Basis hinzugerechnet.
		\return ausgelesener Wert.
		\brief Wirft MagicException bei Fehlschlag
		*/
		std::string ReadString(UINT_PTR address, bool relative = false) const;
		
		/**
		Schreibt sizeof(type) Bytes in den Speicher. 
		\brief Strings sollten damit nicht geschrieben werden!<br/>Wirft MagicException bei Fehlschlag
		\param address Die Adresse, an der wir schreiben m&ouml;chten.
		\param relative Wenn == true dann wird die angegebene Adresse zur Basis hinzugerechnet.
		*/
		template <typename type> void Write(UINT_PTR address, type value, bool relative = false) const
		{
			LPVOID addr = reinterpret_cast<LPVOID>(relative ? address+baseAddress : address);
			W32_CALL(WriteProcessMemory(process, addr, &value, sizeof(value), nullptr));
		}
		
		/**
		Schreibt value.length()+1 Bytes in den Speicher. 
		\param address Die Adresse, an der wir schreiben m&ouml;chten.
		\param relative Wenn == true dann wird die angegebene Adresse zur Basis hinzugerechnet.
		\brief Wirft MagicException bei Fehlschlag
		*/
		void WriteString(UINT_PTR address, std::string value, bool relative = false) const;
	};
}