运行芯片:
ST公司STM32L152VCT6，该控制器是32位Cortex-M3内核的处理器。其主要特征如下：
	32Mhz 最大运行频率
	256Kbytes 程序存储空间；
	32Kbytes SRAM 存储空间；
	8192 byte EEPROM
	128 byte backup register
	12bit ADC模块以及DAC模块；
	3个SPI口；
	2个I2C口；
	3个USART口；


其外接晶体8MHz,然后内部倍频以得到32MHz的系统时钟.
SYSCLK: 32Mhz
AHB: 32Mhz
LSE:32.768Khz
APB1(PCLK1):16Mhz
APB2(PCLK2):16Mhz

进入低功耗后，使用内部晶振262.144KHz
SYSCLK: 262.144KHz
AHB: 131.072KHz
LSE:32.768Khz
APB1(PCLK1):131.072KHz
APB2(PCLK2):131.072KHz

外部存储:
SST25 NorFlash, 大小1M,可进行超过十万次的读写操作

显示器:
OLED 256x64 SEG0~SEG255 COMA0~COMA63 COMB0~COMB63
驱动:SSD1322UR1  (dual mode)
256级灰度
4位写一个像素,一个字节带有两个像素

