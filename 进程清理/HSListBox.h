#pragma once

#ifndef _HSLISTBOX_H_
#define _HSLISTBOX_H_
// CHSListBox
/************************************************************************/
/* 重写CListBox类，为listbox控件实现横向滚动条功能                      */
/************************************************************************/
class CHSListBox : public CListBox
{
	DECLARE_DYNAMIC(CHSListBox)

public:
	CHSListBox();
	virtual ~CHSListBox();

	// 覆盖该方法以便添加水平滚动条
	int AddString( LPCTSTR lpszItem );
	int InsertString( int nIndex, LPCTSTR lpszItem );

	// 计算水平滚动条宽度
	void RefushHorizontalScrollBar( void );
	

protected:
	DECLARE_MESSAGE_MAP()
};


#endif