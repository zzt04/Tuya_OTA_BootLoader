#include "stmflash.h"
#include "stdio.h"
#include <stdint.h>
#include <string.h>

uint8_t nbuff_AAAA[4] = {0xAA,0xAA,0xAA,0xAA};
uint8_t cBuffer[1024];//数据暂存数组

/**
* @brief  读取Flash
* @waing  
* @param  address  读取的地址
* @param  pBuffer  读取的数据
* @param  Numlengh 读取的长度
* @example 1kb = 8位二进制
**/
void ReadFlash(uint32_t address,uint8_t *Nbuffer ,uint32_t  length)
{
	uint32_t temp = 0;
	uint32_t count = 0;
	while(count < length)
	{
		temp = *((volatile uint32_t *)address);//取地址中的值（寄存器命令）32位
		*Nbuffer++ = (temp & 0xff);//取低8位放到数组[0]
		count++;
    if(count >= length)
      break;
	  *Nbuffer++ = (temp >> 8) & 0xff;//右移8位 取低8位放到数组[1]
		count++;
		if(count >= length)
      break;
		*Nbuffer++ = (temp >> 16) & 0xff;//右移8位 取低8位放到数组[2]
		count++;
		if(count >= length)
      break;
		*Nbuffer++ = (temp >> 24) & 0xff;//右移8位 取低8位放到数组[3] 32位数据取完
		count++;
		if(count >= length)
      break;
		address += 4;//地址 + 4 进入下一个32位
	}
}
/**
* @brief  写入Flash
* @waing  
* @param  address  写入的地址
* @param  pBuffer  写入的数据
* @param  Numlengh 写入的长度
* @example 
**/
void WriteFlash(uint32_t address,uint8_t *pBuffer,uint32_t Numlengh)
{
	uint32_t i ,temp;
  HAL_FLASH_Unlock();//flash写操作解锁
	for(i = 0; i < Numlengh;i+= 4)
	{
		temp =  (uint32_t)pBuffer[i+3]<<24;//最高位 0xff000000
		temp |=  (uint32_t)pBuffer[i+2]<<16;//0x00ff0000
		temp |=  (uint32_t)pBuffer[i+1]<<8;//0x0000ff00
		temp |=  (uint32_t)pBuffer[i];//最低位0x000000ff
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,address+i,temp);//将数据按字节写入flash
	}
  HAL_FLASH_Lock();//flash上锁
}
/**
* @brief  擦除1K大小的Flash
* @waing  
* @param  address  擦除的地址
* @example 
**/
int EarseFlash_1K(uint32_t address)
{
		FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;
    
    // 检查地址是否为1K的边界
    if(address % 1024 == 0)
    {
        // 设置擦除参数
        eraseInit.TypeErase = FLASH_TYPEERASE_PAGES; // 页擦除
        eraseInit.PageAddress = address;              // 擦除地址
        eraseInit.NbPages = 1;                        // 擦除1页

        // HAL解锁Flash
        HAL_FLASH_Unlock();

        // 执行擦除
        if(HAL_FLASHEx_Erase(&eraseInit, &pageError) == HAL_OK)
        {
            // HAL锁定Flash
            HAL_FLASH_Lock();
            return 1; // 擦除成功
        }
        else
        {
            // HAL锁定Flash
            HAL_FLASH_Lock();
            return 0; // 擦除失败
        }
    }
    else
    {
        return 0; // 地址不对齐
    }
}
/**
* @brief  读取Flash测试程序
* @waing  
* @param  address  读取的地址
* @example 
**/
uint32_t ReadFlashTest(uint32_t addr)
{
	uint32_t temp = *(__IO uint32_t*)(addr);
	return temp;
}
/**
* @brief  固件升级
* @waing  
* @param  SourceAddress  目标地址
* @param  TargetAddress  源地址
* @example 
**/
void update_firmware(uint32_t SourceAddress,uint32_t TargetAddress)
{
//先读  后擦除  后写  
  uint16_t i;
	volatile uint32_t nK ;
	nK	= (TargetAddress - SourceAddress)/1024;//计算要取数据的大小 单位Kb
	for(i = 0;i < nK;i++)
	{
		memset(cBuffer,0xff,sizeof(cBuffer));
		ReadFlash(TargetAddress+i*0x0400,cBuffer,1024);//每次取源地址1K数据
		EarseFlash_1K(SourceAddress+i*0x0400);//擦除目标地址1K
		WriteFlash(SourceAddress+i*0x0400,cBuffer,1024);//向目的地址写入1K
	}

}

