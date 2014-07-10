/**************************************************************************
MFC����һ���ǳ����õ�CMemFile�࣬�������������д�ļ�һ��������һ���ڴ档
�����������ȱ�����ʹ����MFC����CObject�̳У�����������д��MFC����ʱ�Ͳ���
ʹ�����������ڴ��ļ����ˡ��������Ҹ���MFC��CMemFile��Դ�����д��һ��
CAnsiMemFile�࣬ȥ����CMemFile��ʹ��MFC�Ĳ��֣�ϣ���Դ�����á�

  CAnsiMemFile��ʹ�÷ǳ��򵥣�ֻ�ð�����Ĵ��뱣�浽һ��AnsiMemFile.hͷ�ļ���
  �������û��CPP�ļ�����Ȼ�������VC������#include "AnsiMemFile.h"�Ϳ����ˡ�
  ����Ľӿں�MF��CMemFile�����һ�£����Բο�MSDN��CMemFile����ĵ���
  
	// AnsiMemFile.h: the CAnsiMemFile class.
	//
	// A memory file class without MFC 
	// Version: 1.0
	// Data: 2005-10-27
	// 
	// www.farproc.com
*****************************************************************************/

#if !defined(AFX_ANSIMEMFILE_H__2F568214_9834_4B67_B97E_FFB21CDB61F1__INCLUDED_)
#define AFX_ANSIMEMFILE_H__2F568214_9834_4B67_B97E_FFB21CDB61F1__INCLUDED_

#include <exception>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
//#include 
//#include 
//#include 

//#ifndef UINT

//#endif
namespace LibHowen{
	class CAnsiMemFile
	{
	protected:
		UINT m_nGrowBytes;
		DWORD m_nPosition;
		DWORD m_nBufferSize;
		DWORD m_nFileSize;
		BYTE* m_lpBuffer;
		BOOL m_bAutoDelete;
		
#pragma intrinsic(memcpy)
		BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource,
			UINT nBytes)
		{
			if(0==nBytes)
				return NULL;
			assert(lpMemTarget != NULL);
			assert(lpMemSource != NULL);
			
			//assert(AfxIsValidAddress(lpMemTarget, nBytes));
			//assert(AfxIsValidAddress(lpMemSource, nBytes, FALSE));
			
			return (BYTE*)memcpy(lpMemTarget, lpMemSource, nBytes);
		}
#pragma function(memcpy)
		
		void Free(BYTE *lpMem){free(lpMem);}
		BYTE* Realloc(BYTE *lpMem,DWORD nBytes){
//			//Free(lpMem);
//			if(nBytes<=m_nFileSize){ //Not support
//				//throw exception("Not support");
//				throw "Realloc Error!";
//			}
//			BYTE *pNewBlock=Alloc(nBytes);
//			//copy to new block
//			::memcpy(pNewBlock,lpMem,m_nFileSize);
//			Free(lpMem);
//			return pNewBlock;
			return (BYTE*)realloc(lpMem, (UINT)nBytes);
		}
		BYTE * Alloc(DWORD nBytes){
			//BYTE *pNewBlock=new BYTE[nBytes];
			//assert(pNewBlock!=NULL);
			//return pNewBlock;
			return (BYTE*)malloc((UINT)nBytes);
		}
		
		void GrowFile(DWORD dwNewLen)
		{
			assert(this);
			
			if (dwNewLen > m_nBufferSize)
			{
				// grow the buffer
				DWORD dwNewBufferSize = (DWORD)m_nBufferSize;
				
				// watch out for buffers which cannot be grown!
				assert(m_nGrowBytes != 0);
				//if (m_nGrowBytes == 0)
				// AfxThrowMemoryException();
				
				// determine new buffer size
				while (dwNewBufferSize < dwNewLen)
					dwNewBufferSize += m_nGrowBytes;
				
				// allocate new buffer
				BYTE* lpNew;
				if (m_lpBuffer == NULL)
					lpNew = Alloc(dwNewBufferSize);
				else
					lpNew = Realloc(m_lpBuffer, dwNewBufferSize);
				
				if (lpNew == NULL){
					//AfxThrowMemoryException();
					//throw exception("�����ڴ����");
                                throw "Encounter error when allocating memory!";
				}

				
				m_lpBuffer = lpNew;
				m_nBufferSize = dwNewBufferSize;
			}
			assert(this);
		}
	public:
		enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };
		
		//nGrowBytes ���ļ���Ҫ����ʱ���������ȣ�ÿ������nGrowBytes���ֽڣ�
		CAnsiMemFile(UINT nGrowBytes = 1024)
		{
			assert(nGrowBytes <= UINT_MAX);
			
			m_nGrowBytes = nGrowBytes;
			m_nPosition = 0;
			m_nBufferSize = 0;
			m_nFileSize = 0;
			m_lpBuffer = NULL;
			m_bAutoDelete = TRUE;
		}
		//����ڹ�������Attach
		CAnsiMemFile(const BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0)
		{
			assert(nGrowBytes <= UINT_MAX);
			
			m_nGrowBytes = nGrowBytes;
			m_nPosition = 0;
			m_nBufferSize = nBufferSize;
			m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
			m_lpBuffer=Alloc(nBufferSize);
			memcpy(m_lpBuffer,lpBuffer,nBufferSize);
		//	m_lpBuffer = lpBuffer;
			m_bAutoDelete = TRUE;
		}
		
		~CAnsiMemFile()
		{
			// Close should have already been called, but we check anyway
			if (m_lpBuffer)
				Close();
			assert(m_lpBuffer == NULL);
			
			m_nGrowBytes = 0;
			m_nPosition = 0;
			m_nBufferSize = 0;
			m_nFileSize = 0;
		}
		
		//////////////////////ȡ����
		
		//ȡ�õ�ǰ��дλ��
		DWORD GetPosition() const
		{
			assert(this);
			return m_nPosition;
		}
		//ȡ���ļ��ĵ�ǰ��С
		DWORD GetLength() const
		{
			//		DWORD dwLen, dwCur;
			//		// Seek is a non const operation
			//		CAnsiMemFile* pFile = (CAnsiMemFile*)this;
			//		dwCur = pFile->Seek(0L, current);
			//		dwLen = pFile->SeekToEnd();
			//		pFile->Seek(dwCur, begin);
			//		
			//		return dwLen;
			return m_nFileSize;
		}
		//ȡ���ڴ�����ָ��
		//ע�⣺ֻ������Ҫ�޸ĸ�ָ��ָ������ݡ������Ҫ��д���ļ����ݣ�����Read()��Write()
		const BYTE  * GetPtr()
		{
			return m_lpBuffer;
		}
		
		///////////////////////����
		
		//ָ�����ļ����õ��ڴ�
		//�ö�������ʱ�����ͷŸ��ڴ��
		void Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes)
		{
			assert(m_lpBuffer == NULL);
			
			m_nGrowBytes = nGrowBytes;
			m_nPosition = 0;
			m_nBufferSize = nBufferSize;
			m_nFileSize = nGrowBytes == 0 ? nBufferSize : 0;
			m_lpBuffer = lpBuffer;
			m_bAutoDelete = FALSE;
		}
		//������ļ�������ǰռ�õ��ڴ��Ĺ���
		//���ظ��ڴ���ָ��
		BYTE* Detach()
		{
			BYTE* lpBuffer = m_lpBuffer;
			m_lpBuffer = NULL;
			m_nFileSize = 0;
			m_nBufferSize = 0;
			m_nPosition = 0;
			
			return lpBuffer;
		}
		//�ƶ���дλ�õ��ļ����
		DWORD SeekToEnd()
		{ 
			return Seek(0, CAnsiMemFile::end); 
		}
		//�ƶ���дλ�õ��ļ���ͷ
		void SeekToBegin()
		{ 
			Seek(0, CAnsiMemFile::begin); 
		}
		//�ı��ļ�����
		//�����Ҫ�����·�����ռ�ڴ�
		void SetLength(DWORD dwNewLen)
		{
			assert(this);
			
			if (dwNewLen > m_nBufferSize)
				GrowFile(dwNewLen);
			
			if (dwNewLen < m_nPosition)
				m_nPosition = dwNewLen;
			
			m_nFileSize = dwNewLen;
			assert(this);
		}
		//�ӵ�ǰ��дλ�ö�ȡָ�����ȵ�����
		UINT Read(void* lpBuf, UINT nCount)
		{
			assert(this);
			
			if (nCount == 0)
				return 0;
			
			assert(lpBuf != NULL);
			//assert(AfxIsValidAddress(lpBuf, nCount));
			
			if (m_nPosition > m_nFileSize)
				return 0;
			
			UINT nRead;
			if (m_nPosition + nCount > m_nFileSize)
				nRead = (UINT)(m_nFileSize - m_nPosition);
			else
				nRead = nCount;
			
			Memcpy((BYTE*)lpBuf, (BYTE*)m_lpBuffer + m_nPosition, nRead);
			m_nPosition += nRead;
			
			assert(this);
			
			return nRead;
		}
		//дָ�����ȵ����ݵ��ļ���
		void Write(const void* lpBuf, UINT nCount)
		{
			assert(this);
			
			if (nCount == 0)
				return;
			
			assert(lpBuf != NULL);
			//assert(AfxIsValidAddress(lpBuf, nCount, FALSE));
			
			if (m_nPosition + nCount > m_nBufferSize)
				GrowFile(m_nPosition + nCount);
			
			assert(m_nPosition + nCount <= m_nBufferSize);
			
			Memcpy((BYTE*)m_lpBuffer + m_nPosition, (BYTE*)lpBuf, nCount);
			
			m_nPosition += nCount;
			
			if (m_nPosition > m_nFileSize)
				m_nFileSize = m_nPosition;
			
			assert(this);
		}
		//�ƶ���дλ��
		//lOff �ƶ��ľ���
		//nFrom ָ�������￪ʼ�ƶ�
		//
		//�ڶ�����������Ϊ��
		//enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };
		LONG Seek(LONG lOff, UINT nFrom)
		{
			assert(this);
			assert(nFrom == begin || nFrom == end || nFrom == current);
			
			LONG lNewPos = m_nPosition;
			
			if (nFrom == begin)
				lNewPos = lOff;
			else if (nFrom == current)
				lNewPos += lOff;
			else if (nFrom == end)
				lNewPos = m_nFileSize + lOff;
			else
				return -1;
			
			if (lNewPos < 0){
				//exception e("Seek����");
				throw "Encounter error when seek file!";
			}
			
			m_nPosition = lNewPos;
			
			assert(this);
			return m_nPosition;
		}
		//�ر��ڴ��ļ����ͷ���ռ�õ��ڴ棬�ó���Ϊ0��
		//���ڴ��ļ�������������������д���κ����ݣ�
		//�����������Զ�����Close
		void Close()
		{
			assert((m_lpBuffer == NULL && m_nBufferSize == 0) ||
				!m_bAutoDelete || TRUE);
			assert(m_nFileSize <= m_nBufferSize);
			
			m_nGrowBytes = 0;
			m_nPosition = 0;
			m_nBufferSize = 0;
			m_nFileSize = 0;
			if (m_lpBuffer && m_bAutoDelete)
				Free(m_lpBuffer);
			m_lpBuffer = NULL;
		}
	};

}
/////////////////////////////////////////////////////////////////////////////


#endif // !defined(AFX_ANSIMEMFILE_H__2F568214_9834_4B67_B97E_FFB21CDB61F1__INCLUDED_)
