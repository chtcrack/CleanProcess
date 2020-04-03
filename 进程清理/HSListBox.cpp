// HSListBox.cpp : 实现文件
//

#include "stdafx.h"
#include "HSListBox.h"


// CHSListBox

IMPLEMENT_DYNAMIC(CHSListBox, CListBox)

CHSListBox::CHSListBox()
{

}

CHSListBox::~CHSListBox()
{
}


BEGIN_MESSAGE_MAP(CHSListBox, CListBox)
END_MESSAGE_MAP()



// CHSListBox 消息处理程序

int CHSListBox::AddString( LPCTSTR lpszItem )
{
	int nResult = CListBox::AddString( lpszItem );

	RefushHorizontalScrollBar();

	return nResult;
}

int CHSListBox::InsertString( int nIndex, LPCTSTR lpszItem )
{
	int nResult = CListBox::InsertString( nIndex, lpszItem );

	RefushHorizontalScrollBar();

	return nResult;
}

void CHSListBox::RefushHorizontalScrollBar( void )
{
	CDC *pDC = this->GetDC();
	if ( NULL == pDC )
	{
		return;
	}

	int nCount = this->GetCount();
	if ( nCount < 1 )
	{
		this->SetHorizontalExtent( 0 );
		return;
	}

	int nMaxExtent = 0;
	CString szText;
	for ( int i = 0; i < nCount; ++i )
	{
		this->GetText( i, szText );
		CSize &cs = pDC->GetTextExtent( szText );
		if ( cs.cx > nMaxExtent )
		{
			nMaxExtent = cs.cx;
		}
	}

	this->SetHorizontalExtent( nMaxExtent+5 );
}
