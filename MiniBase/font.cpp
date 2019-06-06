#include "font.h"

#pragma warning( disable: 4996 )
#pragma warning( disable: 4244 )

cFont font;

void cFont::InitText( char* font , int height , int width )
{
	if ( offset.HLType != RENDERTYPE_HARDWARE )
		return;

	HDC hDC;
	HFONT hFont;
	HFONT hOldFont;

	hDC = wglGetCurrentDC();
	g_FontListID = glGenLists( 256 );

	hFont = CreateFontA( height , width , 0 , 0 , FW_NORMAL , FALSE , FALSE , FALSE , FALSE ,
						 OUT_TT_PRECIS , FALSE, PROOF_QUALITY , FALSE , font );

	hOldFont = (HFONT)SelectObject( hDC , hFont );

	if ( !wglUseFontBitmapsA( hDC , 0 , 255 , g_FontListID ) )
		wglUseFontBitmapsA( hDC , 0 , 255 , g_FontListID );

	for ( int i = 0; i < 255; i++ )
	{
		SIZE s;
		char line[2] = { (char)i, 0 };
		GetTextExtentPointA( hDC , line , 1 , &s );
		cwidth[i] = s.cx;
		cheight = s.cy;
	}

	SelectObject( hDC , hOldFont );
	DeleteObject( hFont );
}

void cFont::Print( int x , int y , int r , int g , int b , int a , BYTE flags , char *strText )
{
	if ( offset.HLType != RENDERTYPE_HARDWARE )
	{
		g_Engine.pfnDrawSetTextColor( (float)r / 255.0f , (float)g / 255.0f , (float)b / 255.0f );

		if ( flags & FL_CENTER )
		{
			int iWidth , iHeight;
			g_Engine.pfnDrawConsoleStringLen( strText , &iWidth , &iHeight );
			g_Engine.pfnDrawConsoleString( x - iWidth / 2 , y , strText );
		}
		else
			g_Engine.pfnDrawConsoleString( x , y , strText );

		return;
	}
	else
	{
		y += 10;
	}

	glDisable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

	int drawlen = 0;
	for ( char *p = strText; *p; p++ ) { drawlen += cwidth[*p]; }

	if ( flags & FL_CENTER ) { x -= ( drawlen / 2 ); }
	if ( flags & FL_OUTLINE )
	{
		Render( x - 1 , y - 1 , 0 , 0 , 0 , 200 , strText );
		Render( x + 1 , y - 1 , 0 , 0 , 0 , 200 , strText );
		Render( x - 1 , y + 1 , 0 , 0 , 0 , 200 , strText );
		Render( x + 1 , y + 1 , 0 , 0 , 0 , 200 , strText );
	}
	if ( flags & FL_BACKDROP )
	{
		Render( x , y - 1 , 0 , 0 , 0 , 255 , strText );
		Render( x , y - 1 , 0 , 0 , 0 , 255 , strText );
		Render( x - 1 , y , 0 , 0 , 0 , 255 , strText );
		Render( x - 1 , y , 0 , 0 , 0 , 255 , strText );

		Render( x - 1 , y - 1 , 0 , 0 , 0 , 255 , strText );
		Render( x , y - 1 , 0 , 0 , 0 , 255 , strText );
		Render( x - 1 , y - 1 , 0 , 0 , 0 , 255 , strText );
		Render( x , y - 1 , 0 , 0 , 0 , 255 , strText );
	}
	if ( flags & FL_BOLD )
	{
		Render( x - 1 , y - 1 , r , g , b , a , strText );
		Render( x + 1 , y - 1 , r , g , b , a , strText );
		Render( x - 1 , y + 1 , r , g , b , a , strText );
		Render( x + 1 , y + 1 , r , g , b , a , strText );
	}

	Render( x , y , r , g , b , a , strText );		//draw normally

	glDisable( GL_BLEND );
	glEnable( GL_TEXTURE_2D );
}

void cFont::Render( int x , int y , int r , int g , int b , int a , char *string )
{
	int i = 0;
	while ( x < 0 )
	{
		x += cwidth[string[i]];
		i++; if ( !string[i] ) { return; }
	}

	glColor4ub( r , g , b , a );
	glRasterPos2i( x , y );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST );
	glPushAttrib( GL_LIST_BIT );
	glListBase( g_FontListID );
	glCallLists( native_strlen( string ) - i , GL_UNSIGNED_BYTE , string + i );
	glPopAttrib();
}