
#include <ComGlobal.h>
//#include <windows.h>
/*********************************************************************
 *
 *       _GetData0: BMP, JPEG, GIF
 */
// static int _GetData0(void *p, const U8 **ppData, unsigned NumBytesReq, U32 Off)
// {
//     static char acBuffer[0x200];
//     HANDLE *phFile;
//     DWORD NumBytesRead;

//     phFile = (HANDLE *)p;
//     //
//     // Check buffer size
//     //
//     if (NumBytesReq > sizeof(acBuffer))
//     {
//         NumBytesReq = sizeof(acBuffer);
//     }
//     //
//     // Set file pointer to the required position
//     //
//     SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
//     //
//     // Read data into buffer
//     //
//     ReadFile(*phFile, acBuffer, NumBytesReq, &NumBytesRead, NULL);
//     //
//     // Set data pointer to the beginning of the buffer
//     //
//     *ppData = acBuffer;
//     //
//     // Return number of available bytes
//     //
//     return NumBytesRead;
// }

// /*********************************************************************
//  *
//  *       _GetData1: DTA, PNG
//  */
// static int _GetData1(void *p, const U8 **ppData, unsigned NumBytesReq, U32 Off)
// {
//     HANDLE *phFile;
//     DWORD NumBytesRead;
//     U8 *pData;

//     pData = (U8 *)*ppData;
//     phFile = (HANDLE *)p;
//     //
//     // Set file pointer to the required position
//     //
//     SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
//     //
//     // Read data into buffer
//     //
//     ReadFile(*phFile, pData, NumBytesReq, &NumBytesRead, NULL);
//     //
//     // Return number of available bytes
//     //
//     return NumBytesRead;
// }

// void Emwin_Show_Image(char *File_src, ImageType type, U8 x, U8 y)
// {
//     HANDLE hFile;
//     hFile = CreateFile(File_src, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//     switch (type)
//     {
//     case BMP:
//         /* code */
//         GUI_BMP_DrawEx(_GetData0, (void *)&hFile, x, y);
//         break;
//     case JPEG:
//         GUI_JPEG_DrawEx(_GetData0, (void *)&hFile, x, y);
//         break;
//     case GIF:
//         /* code */
//         GUI_GIF_DrawEx(_GetData0, (void *)&hFile, x, y);
//         break;
//     case DTA:
//         /* code */
//         GUI_DTA_DrawEx(_GetData1, (void *)&hFile, x, y);
//         break;
//     case PNG:
//         /* code */
//         GUI_PNG_DrawEx(_GetData1, (void *)&hFile, x, y);
//         break;

//     default:
//         break;
//     }
// }
