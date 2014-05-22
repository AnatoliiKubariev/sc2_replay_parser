
#include <iostream>
#include <StormLib.h>
#include <vector> 
#include <stdexcept>
#include <cstdint>
#include <fstream>
#include <iterator>
//#include <unique_ptr>

#include <memory>
std::vector<HANDLE> s;
//typedef std::shared_ptr<HANDLE>
typedef std::unique_ptr<HANDLE, decltype(&SFileCloseArchive)> mpq_handle_t;

mpq_handle_t mpq_open(const std::string& file_name)
{
    HANDLE hMpq = NULL;
    // Open the archive
    if(!SFileOpenArchive(L"input.SC2Replay", 0, 0, &hMpq))
        throw std::runtime_error("fail");

    return mpq_handle_t(new HANDLE(hMpq), SFileCloseArchive);
}

std::vector<uint8_t> get_file(HANDLE mpq, const std::string& file_name)
{
    SFILE_FIND_DATA sf;
    auto hFind = SFileFindFirstFile(mpq, file_name.c_str(), &sf, NULL);
    if(!hFind)
        throw std::logic_error("file not found");

    HANDLE hFile;
    if(!SFileOpenFileEx(mpq, sf.cFileName, 0, &hFile))
        throw std::logic_error(std::string(sf.cFileName).c_str());

    std::vector<uint8_t> res(sf.dwFileSize);

    SFileReadFile(hFile, &res[0], res.size(), NULL, NULL);
    SFileCloseFile(hFile);
    return res;
}

int main()
{
    TMPQFile * hf;
    HANDLE hFile;
    BYTE Buffer[100];
    int nError = ERROR_SUCCESS;
    int nFiles = 0;
    int nFound = 0;


    auto mpq_handle = mpq_open("input.SC2Replay");

    auto r = get_file(*mpq_handle, "replay.details");
	std::ofstream ofs("replay.details");
	std::copy(r.begin(), r.end(), std::ostreambuf_iterator<char>(ofs) );

    //// Open the archive
    //if(!SFileOpenArchive(L"input.SC2Replay", 0, 0, &hMpq))
    //{
    //    std::cout<<"fail"<<std::endl;
    //    return -1;
    //}
    SFILE_FIND_DATA sf;
    HANDLE hFind;
    DWORD dwExtraDataSize;
    bool bFound = true;

    hFind = SFileFindFirstFile(*mpq_handle.get(), "*", &sf, "c:\\Tools32\\ListFiles\\ListFile.txt");

	//files from mpq archive
    while(hFind != NULL && bFound != false)
    {
        if(SFileOpenFileEx(*mpq_handle.get(), sf.cFileName, 0, &hFile))
        {
            std::cout<<sf.cFileName<<std::endl;

            hf = (TMPQFile *)hFile;
            SFileReadFile(hFile, Buffer, sizeof(Buffer), NULL, NULL);
            nFiles++;

            if(sf.dwFileFlags & MPQ_FILE_SECTOR_CRC)
            {
                dwExtraDataSize = hf->SectorOffsets[hf->dwSectorCount + 1] - hf->SectorOffsets[hf->dwSectorCount];
                if(dwExtraDataSize != 0)
                    nFound++;
            }

            SFileCloseFile(hFile);
        }

        bFound = SFileFindNextFile(hFind, &sf);
    }



    int a;
    std::cin>>a;
}