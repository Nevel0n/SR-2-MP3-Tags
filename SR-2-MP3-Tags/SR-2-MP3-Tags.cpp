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

// Сімічов Ілля
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
        cout << "Title  : " << string(tagData.title,strnlen(tagData.title, 30)) << endl;
        cout << "Artist : " << string(tagData.artist,strnlen(tagData.artist, 30)) << endl;
        cout << "Album  : " << string(tagData.album,strnlen(tagData.album, 30)) << endl;
        cout << "Year   : " << string(tagData.year, 4) << endl;
        cout << "============================\n" << endl;
    }
}

// Дмитро Бондаренко
namespace Dmytro {
    bool isValidYear(const string& year) {
        if (year.length() != 4)
            return false;
        for (char c : year) {
            if (!isdigit(static_cast<unsigned char>(c)))
                return false;
        }
        return true;
    }
    void setField(char* field, const string& value, size_t maxSize) {
        memset(field, 0, maxSize);
        size_t copySize = min(value.size(), maxSize);
        memcpy(field,value.c_str(), copySize);
    }
    void previewChanges(const ID3v1Tag& tagData) {
        cout << "\n===== PREVIEW =====" << endl;
        cout << "Title  : " << string(tagData.title,strnlen(tagData.title, 30)) << endl;
        cout << "Artist : " << string(tagData.artist,strnlen(tagData.artist, 30))<< endl;
        cout << "Album  : " << string(tagData.album,strnlen(tagData.album, 30))<< endl;
        cout << "Year   : " << string(tagData.year,strnlen(tagData.year, 4))<< endl;
        cout << "===================" << endl;
    }
    bool writeTag(const string& filename, const ID3v1Tag& tagData) {
        fstream file(
            filename,
            ios::in | ios::out | ios::binary
        );
        if (!file.is_open()) {
            cout << "\n[ERROR] Failed to open file for writing." << endl;

            return false;
        }
        file.seekg(0, ios::end);
        if (file.tellg() >= 128) {
            file.seekg(-128, ios::end);
            char checkBuf[3];
            file.read(checkBuf, 3);
            if (strncmp(checkBuf, "TAG", 3) == 0) {
                file.seekp(-128, ios::end);
            }
            else {
                file.clear();
                file.seekp(0, ios::end);
            }
        }
        else {
            file.clear();
            file.seekp(0, ios::end);
        }
        file.write(
            reinterpret_cast<const char*>(&tagData),
            sizeof(ID3v1Tag)
        );
        if (!file.good()) {
            cout << "\n[ERROR] Failed to write metadata." << endl;
            file.close();
            return false;
        }
        file.close();
        cout << "\n[SUCCESS] Metadata saved successfully!" << endl;
        return true;
    }
    void editMenu(ID3v1Tag& tagData, const string& filename) {
        int choice;
        string input;
        while (true) {
            cout << "\n========== EDIT MENU ==========\n";
            cout << "1. Edit Title\n";
            cout << "2. Edit Artist\n";
            cout << "3. Edit Album\n";
            cout << "4. Edit Year\n";
            cout << "5. Preview Changes\n";
            cout << "0. Save and Exit\n";
            cout << "===============================\n";
            cout << "Your choice: ";
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nInvalid input. Enter a number." << endl;
                continue;
            }
            cin.ignore(10000, '\n');
            if (choice == 0)
                break;
            if (choice == 5) {
                previewChanges(tagData);
                continue;
            }
            switch (choice) {
            case 1:
                cout << "Enter new title (max 30 chars): ";
                getline(cin, input);
                if (input.length() > 30) {
                    cout << "Title is too long!" << endl;
                    break;
                }
                setField(tagData.title, input, 30);
                cout << "Title updated." << endl;
                break;
            case 2:
                cout << "Enter new artist (max 30 chars): ";
                getline(cin, input);
                if (input.length() > 30) {
                    cout << "Artist name is too long!" << endl;
                    break;
                }
                setField(tagData.artist, input, 30);
                cout << "Artist updated." << endl;
                break;
            case 3:
                cout << "Enter new album (max 30 chars): ";
                getline(cin, input);
                if (input.length() > 30) {
                    cout << "Album name is too long!" << endl;
                    break;
                }
                setField(tagData.album, input, 30);
                cout << "Album updated." << endl;
                break;
            case 4:
                cout << "Enter year (4 digits): ";
                getline(cin, input);
                if (!isValidYear(input)) {
                    cout << "Invalid year format!"
                         << endl;
                    break;
                }
                setField(tagData.year, input, 4);
                cout << "Year updated."
                     << endl;
                break;
            default:
                cout << "Invalid menu option."
                     << endl;
            }
        }
        previewChanges(tagData);
        char confirm;
        cout << "\nSave changes? (y/n): ";
        cin >> confirm;
        if (confirm == 'y' || confirm == 'Y') {
            writeTag(filename, tagData);
        }
        else {
            cout << "\nChanges were discarded."
                 << endl;
        }
    }
}

int main() {
    string filename;
    cout << "Enter MP3 filename (e.g. track.mp3): ";
    getline(cin, filename);

    ID3v1Tag tagData;
    Ilya::readTag(filename, tagData);
    Ilya::displayTag(tagData);

    char editChoice;
    cout << "Do you want to edit the tags? (y/n): ";
    cin >> editChoice;
    if (editChoice == 'y' || editChoice == 'Y') {
        Dmytro::editMenu(tagData, filename);
        cout << "\nUpdated metadata:\n";
        Ilya::readTag(filename, tagData);
        Ilya::displayTag(tagData);
    }
    return 0;
}