#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cctype>

using namespace std;

#pragma pack(push, 1)
struct ID3v1Tag {
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[28];
    char zeroByte;
    char track;
    char genre;
};
#pragma pack(pop)

namespace Ilya {
    bool readTag(const string& filename, ID3v1Tag& tagData) {
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open()) {
            cout << "[ERROR] Failed to open file for reading: " << filename << endl;
            return false;
        }
        file.seekg(0, ios::end);
        if (file.tellg() < 128) {
            cout << "[INFO] File is too small to contain an ID3v1 tag." << endl;
            memset(&tagData, 0, sizeof(ID3v1Tag));
            strncpy(tagData.tag, "TAG", 3);
            file.close();
            return false;
        }
        file.seekg(-128, ios::end);
        file.read(reinterpret_cast<char*>(&tagData), sizeof(ID3v1Tag));
        file.close();

        if (strncmp(tagData.tag, "TAG", 3) != 0) {
            cout << "[INFO] No ID3v1 tag found. A new tag will be created." << endl;
            memset(&tagData, 0, sizeof(ID3v1Tag));
            strncpy(tagData.tag, "TAG", 3);
            return false;
        }
        return true;
    }

    void displayTag(const ID3v1Tag& tagData) {
        cout << "\n===== CURRENT METADATA =====" << endl;
        cout << "Title  : " << string(tagData.title, strnlen(tagData.title, 30)) << endl;
        cout << "Artist : " << string(tagData.artist, strnlen(tagData.artist, 30)) << endl;
        cout << "Album  : " << string(tagData.album, strnlen(tagData.album, 30)) << endl;
        cout << "Year   : " << string(tagData.year, strnlen(tagData.year, 4)) << endl;
        cout << "============================\n" << endl;
    }
}

int main() {
    string filename;
    cout << "Enter MP3 filename (e.g. track.mp3): ";
    getline(cin, filename);

    ID3v1Tag tagData;
    Ilya::readTag(filename, tagData);
    Ilya::displayTag(tagData);

    return 0;
}
