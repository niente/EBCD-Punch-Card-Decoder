#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <map>
#include <bitset>
#include <unordered_map>

using namespace std;

#define CARD_ROWS 12
#define CARD_COLS 80
#define EBCD_COLS 16
#define EBCD_ROWS 4

class InputFile
{
private:
	string filename;
public:
	ifstream inFile;
	InputFile(string filename);
	~InputFile();
	ifstream & getIfstream();
};

InputFile::InputFile(string filename)
{
	cout << "Opening input file: " << filename << "\n\n";
	inFile.open(filename);
}

ifstream & InputFile::getIfstream()
{
	return inFile;
}

InputFile::~InputFile()
{
	cout << "\nClosing " << filename << "\n";
	inFile.close();
}

class CardData
{
private:
	ifstream &inFileRef;
	string line;
	array<bitset<CARD_COLS>, CARD_ROWS> inputCard;
	array<bitset<CARD_ROWS>, CARD_COLS> transposeCard;
	vector < array<bitset<CARD_ROWS>, CARD_COLS>> cardDeck;
public:
	CardData(ifstream &inFile);
	void readCard();
	void printCard();
	array<bitset<CARD_ROWS>, CARD_COLS> getCard(int index);
	vector < array<bitset<CARD_ROWS>, CARD_COLS>> getCardDeck();
	void setLine();
	void transpose();
	void appendDeck(array<bitset<CARD_ROWS>, CARD_COLS> card);
	int getDeckSize();
};

CardData::CardData(ifstream &inFile)
	: inFileRef(inFile)
{
	cout << "Reading cards...\n";
	while (getline(inFileRef, line))
	{
		readCard();
		transpose();
		appendDeck(transposeCard);
	}
	cout << "Cards in deck: " << getDeckSize() << "\n\n";
}

void CardData::readCard()
{
	for (int i = 0; i < CARD_ROWS; i++)
	{
		setLine();
		bitset<CARD_COLS> bTemp(line);
		inputCard[i] = bTemp;
	}
}

void CardData::setLine()
{
	getline(inFileRef, line);
}

void CardData::transpose()
{
	bitset<CARD_ROWS> bTemp;
	for (int i = 0; i < CARD_COLS; i++)
	{
		for (int j = 0; j < CARD_ROWS; j++)
			bTemp[j] = inputCard[j][CARD_COLS - i - 1]; // bitset index L to R: 79...0
		transposeCard[i] = bTemp;
	}
}

void CardData::appendDeck(array<bitset<CARD_ROWS>, 80> card)
{
	cardDeck.push_back(card);
}

int CardData::getDeckSize()
{
	return cardDeck.size();
}

void CardData::printCard()
{
	cout << "Printing transposed card:\n";
	for (int i = 0; i < CARD_COLS; i++)
		cout << transposeCard[i] << endl;
	cout << "\nPrinting input card:\n";
	for (int i = 0; i < CARD_ROWS; i++)
		cout << inputCard[i] << endl;
}

array<bitset<CARD_ROWS>, CARD_COLS> CardData::getCard(int index)
{
	return cardDeck[index];
}

vector < array<bitset<CARD_ROWS>, CARD_COLS>> CardData::getCardDeck()
{
	return cardDeck;
}

class PunchCardCharacters
{
private:
	array< array<string, EBCD_COLS>, EBCD_ROWS> EBCD_PunchCard;
	unordered_map<bitset<3>, string> bitToRowName;
	array<string, EBCD_ROWS> rowNames;
	array<string, EBCD_COLS> colNames;
	unordered_map<bitset<9>, string> bitToColName;
	map<pair<string, string>, string> punchCardMap;
	vector<bitset<9> > punctuationList;
public:
	PunchCardCharacters();
	void buildBitsetMap();
	pair<string, string> buildKey(bitset<12> inputCol);
	void buildCardArray();
	void buildCardMap();
	string getLetter(bitset<12> cardCol);
};

void PunchCardCharacters::buildBitsetMap()
{
	rowNames = { "Y", "X", "0", "-" };
	bitset<3> bRowKey = 1;
	for (int i = 1; i <= EBCD_ROWS; i++)
	{
		bitToRowName.insert({ bRowKey, rowNames[i - 1] });
		bRowKey <<= 1;
	}

	colNames = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", "82", "83", "84", "85", "86", "87" };
	bitset<9> bColKey = 1;
	for (int i = 1; i <= 10; i++)
	{
		bitToColName.insert({ bColKey, colNames[i - 1] });
		bColKey <<= 1;
	}
	// manually inserting punctuation columns
	bitset<9> bTemp("010000010");
	for (int i = 1; i <= 6; i++)
	{
		punctuationList.push_back(bTemp);
		bTemp.reset(i);
		bTemp.set(i+1);
	}

	for (int i = 10, j = 0; i < EBCD_COLS; i++, j++)
	{
		bitToColName.insert({ punctuationList[j], colNames[i] });
	}

}

pair<string, string> PunchCardCharacters::buildKey(bitset<12> inputCol)
{
	pair<string, string> key_pair;
	bitset<3> rowName;
	for (int i = 0; i < 3; i++)
		rowName[i] = inputCol[i];
	key_pair.first = bitToRowName[rowName];

	bitset<9> colName;
	for (int i = 0; i < 9; i++)
		colName[i] = inputCol[i + 3];
	key_pair.second = bitToColName[colName];

	return key_pair;
}

void PunchCardCharacters::buildCardArray()
{
	EBCD_PunchCard = { {
		{ " ", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", "#", "@", "'", "=", "\"" },
		{ "&", "A", "B", "C", "D", "E", "F", "G", "H", "I", "[", ".", "<", "(", "+", "!" },
		{ "-", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "]", "$", "*", ")", ";", "^" },
		{ "0", "/", "S", "T", "U", "V", "W", "X", "Y", "Z", "\\", ",", "%", "_", ">", "?" }
	} };
}

void PunchCardCharacters::buildCardMap()
{
	pair<string, string> key;
	vector<string> rowNames = { "-", "Y", "X", "0" };
	vector<string> colNames = { "-", "1", "2", "3", "4", "5", "6", "7", "8", "9", "82", "83", "84", "85", "86", "87" };
	for (int i = 0; i < EBCD_ROWS; i++)
	{
		for (int j = 0; j < EBCD_COLS; j++)
		{
			key.first = rowNames[i];
			key.second = colNames[j];
			punchCardMap[key] = EBCD_PunchCard[i][j];
		}
	}
}

PunchCardCharacters::PunchCardCharacters()
{
	buildBitsetMap();
	buildCardArray();
	buildCardMap();
}

string PunchCardCharacters::getLetter(bitset<CARD_ROWS> cardCol)
{
	return punchCardMap[buildKey(cardCol)]; // returns letter as a string
}

class PunchCardDecoder
{
private:
	bitset<CARD_ROWS> col;
	vector<string> allCardText;
	vector < array<bitset<CARD_ROWS>, CARD_COLS>> cardDeck;
	PunchCardCharacters cardTemplate;
public:
	PunchCardDecoder(vector < array<bitset<CARD_ROWS>, CARD_COLS> > cardDeck, PunchCardCharacters cardTemplate);
	vector<string> getDeckText();
	void printAllCardText();
};

PunchCardDecoder::PunchCardDecoder(vector<array<bitset<CARD_ROWS>, CARD_COLS> > cardDeck, PunchCardCharacters cardTemplate)
{
	cardDeck = cardDeck;
	cardTemplate = cardTemplate;
	string cardText;
	for (int i = 0; i < cardDeck.size(); i++)
	{
		cardText = "";
		for (int j = 0; j < CARD_COLS; j++)
		{
			cardText += cardTemplate.getLetter(cardDeck[i][j]);
		}
		allCardText.push_back(cardText);
	}
}

vector<string> PunchCardDecoder::getDeckText()
{
	return allCardText;
}

void PunchCardDecoder::printAllCardText()
{
	for (auto i = allCardText.begin(); i != allCardText.end(); ++i)
		cout << *i << endl;
}

int main()
{
	InputFile file("PumchCards.txt");
	CardData inputCard(file.getIfstream());
	PunchCardCharacters cardTemplate;
	PunchCardDecoder decode(inputCard.getCardDeck(), cardTemplate);
	decode.printAllCardText();
	system("pause");
	return 0;
}