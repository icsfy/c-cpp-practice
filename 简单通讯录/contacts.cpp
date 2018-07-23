#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const string CONTACTS_FILENAME("contacts.dat");

enum OPTIONS {
	AddContacts = 1,
	SearchNumber,
	ContactsList,
	ContactsData,
	DeleteContacts,
	SaveContacts,
	ExitSystem
};

struct Contacts {
	string name;
	string number;
};

vector<Contacts> contacts;

void showMenu()
{
	cout << "选择功能：\n"
		"\t" << AddContacts << ". 添加联系人\n"
		"\t" << SearchNumber << ". 查找号码\n"
		"\t" << ContactsList << ". 联系人列表\n"
		"\t" << ContactsData << ". 联系人数据\n"
		"\t" << DeleteContacts << ". 删除联系人\n"
		"\t" << SaveContacts << ". 保存联系人\n"
		"\t" << ExitSystem << ". 退出系统\n";
}

void clearInput()
{
	while (cin.get() != '\n');
}

void addContacts()
{
	cout << "\t--- 添加联系人 ---" << endl;
	Contacts c;
	cout << "联系人名字: ";
	getline(cin, c.name);
	cout << "联系人号码: ";
	getline(cin, c.number);
	contacts.push_back(c);
	cout << "--- " << c.name << " : " << c.number << " 添加完成 ---" << endl;
}

void searchNumber()
{
	cout << "\t--- 查找号码 ---" << endl;
	string number;
	cout << "你要查找的号码为: ";
	getline(cin, number);
	vector<Contacts> numContacts;
	for (auto v = contacts.begin(); v != contacts.end(); ++v) {
		if (v->number == number) {
			numContacts.push_back(*v);
		}
	}
	if (numContacts.size() == 0) {
		cout << "找不到联系人" << endl;
	} else {
		cout << "共找到" << numContacts.size() << "个联系人: " << endl;
		for (auto v = numContacts.begin(); v != numContacts.end(); ++v) {
			cout << "\t" << v->name << endl;
		}
	}
}

void contactsList()
{
	cout << "\t--- 联系人列表 ---" << endl;
	if (contacts.size() == 0) {
		cout << "\t\t空" << endl;
		return;
	}
	int i = 0;
	for (auto v = contacts.begin(); v != contacts.end(); ++v) {
		cout << "\t" << ++i << ". " << v->name << endl;
	}
}

void contactsData()
{
	cout << "\t--- 联系人数据 ---" << endl;
	string name;
	cout << "你要查找的联系人为: ";
	getline(cin, name);
	vector<Contacts> nameContacts;
	for (auto v = contacts.begin(); v != contacts.end(); ++v) {
		if (v->name == name) {
			nameContacts.push_back(*v);
		}
	}
	if (nameContacts.size() == 0) {
		cout << "找不到联系人" << endl;
	}
	else {
		cout << "共找到" << nameContacts.size() << "个号码: " << endl;
		for (auto v = nameContacts.begin(); v != nameContacts.end(); ++v) {
			cout << "\t" << v->name << " : " << v->number << endl;
		}
	}
}

void deleteContacts()
{
	cout << "\t--- 删除联系人 ---" << endl;
	string name;
	cout << "你要查找的联系人为: ";
	getline(cin, name);
	int num = contacts.size();
	for (auto v = contacts.begin(); v != contacts.end();) {
		if (v->name == name) {
			v = contacts.erase(v);
		} else {
			++v;
		}
	}
	cout << "--- 共删除" << num - contacts.size() << "个联系人" << endl;
}

void exitSystem()
{
	cout << "Bye ..." << endl;
	exit(0);
}

void loadContacts()
{
	ifstream fin(CONTACTS_FILENAME);
	if (fin.is_open()) {
		while (fin.good()) {
			Contacts c;
			getline(fin, c.name);
			if (c.name == "")
				continue;
			getline(fin, c.number);
			contacts.push_back(c);
		}
		fin.close();
		cout << "读取到" << contacts.size() << "个联系人" << endl;
		return;
	}
	cout << "联系人初始化为空" << endl;
}

void saveContacts()
{
	ofstream fout(CONTACTS_FILENAME);
	if (fout.is_open()) {
		for (auto v = contacts.begin(); v != contacts.end(); ++v) {
			fout << v->name << endl << v->number << endl;
		}
		fout.close();
		cout << "--- 保存完成 ---" << endl;
		return;
	}
	cout << "--- 文件打开出错，保存失败 ---" << endl;
}

void chooseOption(int option)
{
	switch (option) {
	case AddContacts:
		addContacts();
		break;
	case SearchNumber:
		searchNumber();
		break;
	case ContactsList:
		contactsList();
		break;
	case ContactsData:
		contactsData();
		break;
	case DeleteContacts:
		deleteContacts();
		break;
	case SaveContacts:
		saveContacts();
		break;
	case ExitSystem:
		exitSystem();
		break;
	default:
		cerr << "选项不正确，请输入数字" << endl;
		break;
	}
}

int main()
{
	char option = 0;
	cout << "\t--- 通讯录 ---" << endl;
	loadContacts();
	while (true) {
		cout << "----------------" << endl;
		showMenu();
		cout << ">> ";
		option = cin.get();
		clearInput();
		chooseOption(option - '0');
	}
	return 0;
}