//#pragma once 这个可以替代下面的条件编译：#ifndef
#ifndef __DATA____IO___
#define __DATA____IO___

#include <algorithm>//算法库
#include <vector> //向量容器
#include <fstream> //文件流
#include "afxwin.h"//windows控件

using namespace std;

//字段类型枚举：整型、实型、日期时间型、字串型
enum EField {eInt, eDouble, eDataTime, eString};
enum ESelect {eEqual, eGreater, eLess, eGreaterOrEqual, eLessOrEqual, eLike};

//字段：字段名 + 字段数据类型
struct CField{
	CString sFieldName;
	EField eFieldType;
};


//数据集，模板类
template <class T>
class CDataset{
private:
	CDataset &operator =(const CDataset &){return *this;}//不允许赋值运算

	bool isSelData(T &data, CString sNameField, ESelect eSelect, CString sVal);

protected:
	vector<CField> aFields;
	vector<T> aData;

	CString sFileName;
public:
	virtual ~CDataset(){aFields.clear(); aData.clear();}

public:
	vector<T> &getData(){
		return aData;
	}

	vector<CField> &getFields(){
		return aFields;
	}

public:
	//基于字段名获取字段信息
	virtual const CField &getFieldByName(CString sFieldName);

	//排序:参数 是用于比较 的函数指针
	virtual void dataOrder(bool comp(const T &b1, const T &b2));

	//基于键值 获取 记录数据指针
	virtual T *getItemByKeyVal(CString sKeyField, int nKeyVal);

	//保存全部数据到文件，返回 保存的 记录数
	virtual int saveAllDataToFile();

	//基于键值删除数据
	virtual bool deleteByKeyVal(CString sKeyField, int nKeyVal);

	//筛选sNameField的值满足eSelect（sVal）的数据,得到的数据集存放到 aDataset
	virtual void selectData(vector<T> &aDataset,CString sNameField, ESelect eSelect, CString sVal);

	//记录是否存在
	virtual bool itemExists(CString sNameField, CString sVal);

	//记录是否存在
	virtual bool itemExists(CString sNameField, int nVal);

	//获取某字段的最大值，如Id【该字段必须是整型，否则结果异常】
	virtual int getMaxVal(CString sNameField);

	//对字段sNameField模糊查询，结果保存到aDataset中
	//如sNameField="书名" sValLike="数",则 高等数学、线性代数 均满足
	virtual void likeQuery(CString sNameField, CString sValLike, vector<T> &aDataset);

	//精确查询，如：查找 出版社 为 “西南交通大学出版社” 的全部图书，自己弄

	//保存一条记录到文件
	virtual bool saveOneItemToFile(T *);

	//读取全部数据到 aData 中
	virtual int readAllData();
	
	//制表：把m_pData中的数据填写到列表控件 pListCtrl 中
	virtual void dataToListCtrl(CListCtrl *pListCtrl, vector<T> *m_pData = NULL);

	//填写表头
	virtual void doListHeader(CListCtrl *pListCtrl);


//静态成员函数
public:		
	//根据单元格文字长度自动调整列宽
	static void listAutoWidth(CListCtrl *pListCtrl, int nColSpace, int nMinWid);

   //清除列表控件 pListCtrl 里显示的数据
	static void clearCListCtrl(CListCtrl *pListCtrl);

   //获得EXE所在位置
	static CString getExePath();
};


//接口类：一条记录，所有记录均应从此派生，以强制子类实现接口
class CDataItem{
public:
	//返回第i个字段的值
	virtual CString operator[](int i) = 0;

	//返回字段sFieldName的值
	virtual CString operator[](CString sFieldName) = 0;

	//析构没写代码，为什么必须写上呢？
	virtual ~CDataItem(){}
};

/*******************************************************************
以下封装的2个类 CBookData、CBookDataset用于管理【图书基本信息】
其它的如;用户信息、借阅信息等等请自行模仿处理
********************************************************************/
//图书数据(一条记录)【为保证每条记录字节数定长，字串使用字符数组，而不是CString】
class CBookData : public CDataItem{
public:
	int    nBookId;
	char   sBookname[31];
	char   sBookNo[31];
	char   sAuthor[11];
	char   sPubHouse[31];
	double fPrice; //图书单价

	CBookData(int id = 0, char *name="",char *no="",char *auth="",char *pub="",double price=0){
		strcpy_s(sBookname, 31, name);
		strcpy_s(sBookNo,   31, no);
		strcpy_s(sAuthor,   11, auth);
		strcpy_s(sPubHouse, 31, pub);

		nBookId  =  id;  
		fPrice   =  price;
	}

	virtual ~CBookData(){}

//父类纯虚函数的实现
public:
	//返回第i个字段的值
	virtual CString operator[](int i){
		CString s;
		switch(i){
		case 0:  s.Format("%d", nBookId);   break;
		case 5:  s.Format("%.2f", fPrice);  break;
		case 1:  s = sBookname;             break;
		case 2:  s = sBookNo;               break;
		case 3:  s = sAuthor;               break;
		case 4:  s = sPubHouse;             break;
		default: throw CString("CBookData：字段越界");
		}

		return s;
	}

	//返回字段sFieldName的值
	virtual CString operator[](CString sFieldName){
		//此处必须和CBookDataset构造函数里保持一致
		char *aName[6] = {"Id","书名","书号","作者","出版社","单价"};

		int nIndex = 0;
		for(int i = 0; i < 6; i++){
			if(aName[i] == sFieldName){
				return (*this)[i];
			}
		}

		throw CString("CBookData\n非法字段名：" + sFieldName);
	}

	

//用于排序的静态比较成员函数
public:
	static bool compByIdAsc(const CBookData &b1, const CBookData &b2){
		return b1.nBookId < b2.nBookId; //升序
	}
	static bool compByIdDec(const CBookData &b1, const CBookData &b2){
		return b1.nBookId > b2.nBookId; //降序
	}
	static bool compByNameAsc(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sBookname, b2.sBookname) < 0; //升序
	}
	static bool compByNameDec(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sBookname, b2.sBookname) > 0; //降序
	}
	static bool compByNoAsc(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sBookNo, b2.sBookNo) < 0; //升序
	}
	static bool compByNoDec(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sBookNo, b2.sBookNo) > 0; //降序
	}
	static bool compByAuthorAsc(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sAuthor, b2.sAuthor) < 0; //升序
	}
	static bool compByAuthorDec(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sAuthor, b2.sAuthor) > 0; //降序
	}
	static bool compByPubHouseAsc(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sPubHouse, b2.sPubHouse) < 0; //升序
	}
	static bool compByPubHouseDec(const CBookData &b1, const CBookData &b2){
		return strcmp(b1.sPubHouse, b2.sPubHouse) > 0; //降序
	}
	static bool compByPriceAsc(const CBookData &b1, const CBookData &b2){
		return b1.fPrice < b2.fPrice; //升序
	}
	static bool compByPriceDec(const CBookData &b1, const CBookData &b2){
		return b1.fPrice > b2.fPrice; //降序
	}
};

//图书数据集（若干条记录） 【注意：基类是模板类】
class CBookDataset : public CDataset<CBookData>{
public:
	CBookDataset();//构造函数 完成 aFields 的初始化

	virtual ~CBookDataset(){}

};
/*******************************************************************
以下封装的2个类 CAccountData、CAccountDataset用于管理【账户基本信息】
********************************************************************/
//图书数据(一条记录)【为保证每条记录字节数定长，字串使用字符数组，而不是CString】
class CAccountData : public CDataItem{
public:
	int    Account;
	int    nameID;
	char   password[31];
	char   name[31];

	CAccountData(int Acc = 0, char *word="",int ID = 0,char *nameword=""){
		strcpy_s(password, 31, word);
		strcpy_s(name, 31, nameword);
		nameID   =  ID;
		Account  =  Acc;
	}

	virtual ~CAccountData(){}

//父类纯虚函数的实现
public:
	//返回第i个字段的值
	virtual CString operator[](int i){
		CString s;
		switch(i){
		case 0:  s.Format("%d",  Account);   break;
		case 1:  s = password;             break;
		case 2:  s.Format("%d",  nameID);   break;
		case 3:  s = name;             break;
		default: throw CString("CBookData：字段越界");
		}

		return s;
	}

	//返回字段sFieldName的值
	virtual CString operator[](CString sFieldName){
		//此处必须和CAccountDataset构造函数里保持一致
		char *aName[4] = {"账号","密码","学号","姓名"};

		int nIndex = 0;
		for(int i = 0; i < 4; i++){
			if(aName[i] == sFieldName){
				return (*this)[i];
			}
		}

		throw CString("CBookData\n非法字段名：" + sFieldName);
	}

	

//用于排序的静态比较成员函数
public:
	static bool compByAccountAsc(const CAccountData &b1, const CAccountData &b2){
		return b1.Account < b2.Account; //升序
	}
	static bool compByAccountDec(const CAccountData &b1, const CAccountData &b2){
		return b1.Account > b2.Account; //降序
	}
	//....
};
//用户数据集（若干条记录） 【注意：基类是模板类】
class CAccountDataset : public CDataset<CAccountData>{
public:
	CAccountDataset();//构造函数 完成 aFields 的初始化

	virtual ~CAccountDataset(){}

};
/*******************************************************************
以下封装的2个类 CReaderData、CReaderDataset用于管理【读者借阅基本信息】
********************************************************************/
//图书数据(一条记录)【为保证每条记录字节数定长，字串使用字符数组，而不是CString】
class CReaderData : public CDataItem{
public:
	int    Account;
	char   password[31];
	int    nBookId;
	char   state[31];

	CReaderData(int    Acc=0,char   *word="",int id = 0, char   *ing=""){
		strcpy_s(password, 31, word);
		strcpy_s(state, 31, ing);
		Account  =  Acc;
		nBookId  =  id;  
	}

	virtual ~CReaderData(){}

//父类纯虚函数的实现
public:
	//返回第i个字段的值
	virtual CString operator[](int i){
		CString s;
		switch(i){
		case 0:  s.Format("%d", Account);   break;
		case 1:  s = password;              break;
		case 2:  s.Format("%d", nBookId);   break;
		case 3:  s = state;                 break;
		default: throw CString("CBookData：字段越界");
		}

		return s;
	}

	//返回字段sFieldName的值
	virtual CString operator[](CString sFieldName){
		//此处必须和CReaderDataset构造函数里保持一致
		char *aName[4] = {"账号","密码","图书Id","状态"};

		int nIndex = 0;
		for(int i = 0; i < 4; i++){
			if(aName[i] == sFieldName){
				return (*this)[i];
			}
		}

		throw CString("CBookData\n非法字段名：" + sFieldName);
	}

	

//用于排序的静态比较成员函数
public:
	static bool compByAccountAsc(const CReaderData &b1, const CReaderData &b2){
		return b1.Account < b2.Account; //升序
	}
	static bool compByAccountDec(const CReaderData &b1, const CReaderData &b2){
		return b1.Account > b2.Account; //降序
	}
	
};

//图书数据集（若干条记录） 【注意：基类是模板类】
class CReaderDataset : public CDataset<CReaderData>{
public:
	CReaderDataset();//构造函数 完成 aFields 的初始化

	virtual ~CReaderDataset(){}

};


#endif
