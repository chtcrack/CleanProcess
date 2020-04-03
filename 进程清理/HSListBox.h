#pragma once

#ifndef _HSLISTBOX_H_
#define _HSLISTBOX_H_
// CHSListBox
/************************************************************************/
/* ��дCListBox�࣬Ϊlistbox�ؼ�ʵ�ֺ������������                      */
/************************************************************************/
class CHSListBox : public CListBox
{
	DECLARE_DYNAMIC(CHSListBox)

public:
	CHSListBox();
	virtual ~CHSListBox();

	// ���Ǹ÷����Ա����ˮƽ������
	int AddString( LPCTSTR lpszItem );
	int InsertString( int nIndex, LPCTSTR lpszItem );

	// ����ˮƽ���������
	void RefushHorizontalScrollBar( void );
	

protected:
	DECLARE_MESSAGE_MAP()
};


#endif