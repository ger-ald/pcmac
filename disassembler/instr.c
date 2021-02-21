
/*
** These strings are needed by the disassembly.
** The names are the same as they are in the
** assembly definition.
*/
char *_Ss[]={
	"B","C","D","E","H","L","(HL)","A"};
#define M_SS	07
char *_SDD[]={
	"BC","DE","HL","SP"};
#define M_SDD	03
char *_SQQ[]={
	"BC","DE","HL","AF"};
#define M_SQQ	03
char *_SPP[]={
	"BC","DE","IX","SP"};
#define M_SPP	03
char *_SRR[]={
	"BC","DE","IY","SP"};
#define M_SRR	03
char *_SXX[]={
	"IX","IY"};
#define M_SXX	01
char *_SDB[]={
	"BC","DE"};
#define M_SDB	01
char *_SIR[]={
	"I","R"};
#define M_SIR	01

char *_SCE[]={
	"NZ","Z","NC","C","PO","PE","P","M"};
#define M_SCE	07
char *_SCC[]={
	"NZ","Z","NC","C"};
#define M_SCC	03

char *_SAR[]={
	"ADD","ADC","SUB","SBC","AND","XOR","OR","CP"};
#define M_SAR	07
char *_SROT[]={
	"RLC","RRC","RL","RR","SLA","SRA","SLI","SRL"};
#define M_SROT	07
char *_SROTA[]={
	"RLCA","RRCA","RLA","RRA"};
#define M_SROTA	03
char *_SBIT[]={
	"BIT","RES","SET"};
#define M_SBIT	03
char *_SID[]={
	"INC","DEC"};
#define M_SID	01
char *_SGC[]={
	"DAA","CPL","SCF","CCF"};
#define M_SGC	03
char *_SADS[]={
	"ADD","ADC","__GAP__","SBC"};
#define M_SADS	03

/*
** This function disassembles one instruction.
** returns the number of the bytes were worked up
**
** The dissassembled line is in 'line'.
** The code is in 'code' and the actual place
** is pointed by 'dollar'.
*/

unsigned int dis(){
	unsigned int
	byte,
	reg1,
	reg2,
	reg,
	pre,
	offset,
	num,
	xx,
	rpair,
	ir,
	address,
	instr,
	cond
	;
	char *label(),*rlabel();
	void data(),cont(),rcont();

	for(;;)
	{/* LD reg,reg */
		byte = code( dollar ) ;
		if( (byte&(~((M_SS<<3)+M_SS))) != 0x40 )break;

		reg2 = byte&0x7;
		reg1 = (byte>>3)&0x7;
		if( reg1 == 6 && reg2 == 6 )break;
		sprintf(name,"LD\t%s,%s",_Ss[reg1],_Ss[reg2]);
		cont( dollar+1 );
		return 1;
	}

	for(;;)
	{/* LD reg,number */
		byte =  code(dollar) ;
		if( (byte&(~(M_SS<<3))) != 6 )break;

		num = code(dollar+1);
		reg = (byte>>3)&M_SS;
		sprintf(name,"LD\t%s,0%02Xh",_Ss[reg],num);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LD reg,(IX\IY + offset) */
		pre = code(dollar);
		if( pre!=0xfd && pre!=0xdd )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SS<<3)))!=0x46 )break;

		xx = (pre>>5)&M_SXX;
		reg = (byte>>3)&M_SS;
		offset = code(dollar+2);
		sprintf(name,"LD\t%s,(%s+0%02Xh)",_Ss[reg],_SXX[xx],offset);
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD (IX\IY+offset),reg */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( (byte&(~M_SS)) != 0x70 )break;

		xx = (pre>>5)&M_SXX;
		reg = byte&07;
		offset = code(dollar+2);
		sprintf(name,"LD\t(%s+0%02Xh),%s",_SXX[xx],offset,_Ss[reg]);
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD (IX\IY+offset),numeric */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0x36 )break;

		xx = (pre>>5)&M_SXX;
		offset = code(dollar+2);
		num = code(dollar+3);
		sprintf(name,"LD\t(%s+0%02Xh),0%02Xh",_SXX[xx],offset,num);
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD A,(BC/DE) */
		byte = code(dollar);
		if( (byte&(~(M_SDB<<4))) != 0xa )break;

		rpair = (byte>>4)&1;
		sprintf(name,"LD\tA,(%s)",_SDB[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* LD (BC\DE),A */
		byte = code(dollar);
		if( (byte&(~(M_SDB<<4))) != 2 )break;

		rpair = (byte>>4)&1;
		sprintf(name,"LD\t(%s),A",_SDB[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* LD A,I\R */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SIR<<3))) != 0x57 )break;

		ir = (byte>>3)&M_SIR;
		sprintf(name,"LD\tA,%s",_SIR[ir]);
		cont( dollar+2);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LD I\R,A */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SIR<<3))) != 0x47 )break;

		ir =(byte>>3)&M_SIR;
		sprintf(name,"LD\t%s,A",_SIR[ir]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LD A,(address) */
		byte = code(dollar);
		if( byte != 0x3a )break;
		address = code(dollar+1)+0x100*code(dollar+2);
		data(address);
		sprintf(name,"LD\tA,(%s)",label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD (address),A */
		byte = code(dollar);
		if( byte != 0x32 )break;

		address = code(dollar+1)+0x100*code(dollar+2);
		data(address);
		sprintf(name,"LD\tA,(%s)",label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LDI */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa0 )break;

		sprintf(name,"LDI");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LDIR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb0 )break;

		sprintf(name,"LDIR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LDD */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa8 )break;

		sprintf(name,"LDD");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LDDR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb8 )break;

		sprintf(name,"LDDR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* LD rpair,number */
		byte = code(dollar);
		if( (byte&(~(M_SDD<<4))) != 1 )break;

		address = code(dollar+1)+0x100*code(dollar+2);
		rpair = (byte>>4)&M_SDD;
		sprintf(name,"LD\t%s,%s",_SDD[rpair],label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD IX\IY,number */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( byte != 0x21 )break;

		xx = (pre>>5)&M_SXX;
		address = code(dollar+2)+0x100*code(dollar+3);
		sprintf(name,"LD\t%s,%s",_SXX[xx],label(address));
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD HL,(address) */
		byte = code(dollar);
		if( byte != 0x2a )break;
		address = code(dollar+1) + 0x100*code(dollar+2);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\tHL,(%s)",label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD regpair,(address) */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SDD<<4))) !=0x4b )break;

		rpair = (byte>>4)&M_SDD;
		address = code(dollar+2) + 0x100*code(dollar+3);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\t%s,(%s)",_SDD[rpair],label(address));
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD (address),HL */
		byte = code(dollar);
		if( byte != 0x22 )break;

		address= code(dollar+1) + 0x100*code(dollar+2);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\t(%s),HL",label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* LD (address),regpair */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SDD<<4))) != 0x43 )break;

		rpair = (byte>>4)&M_SDD;
		address = code(dollar+2) + 0x100*code(dollar+3);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\t(%s),%s",_SDD[rpair],label(address));
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD IX\IY,(address) */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0x2a )break;

		xx = (pre>>5)&M_SXX;
		address = code(dollar+2) + 0x100*code(dollar+3);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\t%s,(%s)",_SXX[xx],label(address));
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD (address),IX\IY */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0x22 )break;

		xx = (pre>>5)&M_SXX;
		address = code(dollar+2) + 0x100*code(dollar+3);
		data(address);
		data(address+1);
		word(address);
		sprintf(name,"LD\t(%s),%s",label(address),_SXX[xx]);
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* LD SP,HL */
		byte = code(dollar);
		if( byte != 0xf9 )break;

		sprintf(name,"LD\tSP,HL");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* LD SP,IX\IY */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0xf9 )break;

		xx = (pre>>5)&M_SXX;
		sprintf(name,"LD\tSP,%s",_SXX[xx]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* PUSH rpair */
		byte = code(dollar);
		if( (byte&(~(M_SQQ<<4))) != 0xc5 )break;

		rpair = (byte>>4)&M_SQQ;
		sprintf(name,"PUSH\t%s",_SQQ[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* POP regpair */
		byte = code(dollar);
		if( (byte&(~(M_SQQ<<4))) != 0xc1 )break;

		rpair = (byte>>4)&M_SQQ;
		sprintf(name,"POP\t%s",_SQQ[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* PUSH IX\IY */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0xe5 )break;

		xx = (pre>>5)&M_SXX;
		sprintf(name,"PUSH\t%s",_SXX[xx]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* POP IX\IY */
		pre = code(dollar);
		if( pre != 0xfd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( byte != 0xe1 )break;

		xx = (pre>>5)&M_SXX;
		sprintf(name,"POP\t%s",_SXX[xx]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* EX DE,HL */
		byte = code(dollar);
		if( byte != 0xeb )break;

		sprintf(name,"EX\tDE,HL");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* EX AF,AF */
		byte = code(dollar);
		if( byte != 0x8 )break;

		sprintf(name,"EX\tAF,AF");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* EX (SP),HL */
		byte = code(dollar);
		if( byte != 0xe3 )break;

		sprintf(name,"EX\t(SP),HL");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* EX SP,(IX\IY) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( byte != 0xe3 )break;

		xx = (pre>>5)&M_SXX;
		sprintf(name,"EX\tSP,(%s)",_SXX[xx]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* EXX */
		byte = code(dollar);
		if( byte != 0xd9 )break;

		sprintf(name,"EXX");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* ADD HL,rpair */
		byte = code(dollar);
		if( (byte&(~(M_SDD<<4))) != 9 )break;

		rpair = (byte>>4)&M_SDD;
		sprintf(name,"ADD\tHL,%s",_SDD[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* ADD IX\IY,rpair */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SDD<<4))) != 9 )break;

		xx = (pre>>5)&M_SXX;
		rpair = (byte>>4)&M_SDD;
		sprintf(name,"ADD\t%s,%s",_SXX[xx],_SDD[rpair]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* ADC HL,rpair */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SDD<<4))) != 0x4a )break;

		rpair = (byte>>4)&M_SDD;
		sprintf(name,"ADC\tHL,%s",_SDD[rpair]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* SBC HL,rpair */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SDD<<4))) != 0x42 )break;

		rpair = (byte>>4)&M_SDD;
		sprintf(name,"SBC\tHL,%s",_SDD[rpair]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* INC\DEC rpair */
		byte = code(dollar);
		if( (byte&(~((M_SID<<3)|(M_SDD<<4)))) != 0x03 )break;

		instr = (byte>>3)&M_SID;
		rpair = (byte>>4)&M_SDD;
		sprintf(name,"%s\t%s",_SID[instr],_SDD[rpair]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* 8 bit arithmetic */
		byte = code(dollar);
		if( (byte&(~((M_SAR<<3)|(M_SS)))) != 0x80 )break;

		instr = (byte>>3)&M_SAR;
		reg   =  byte &M_SS;
		sprintf(name,"%s\t%s",_SAR[instr],_Ss[reg]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* 8 bit arithmetic number. */
		byte = code(dollar);
		if( (byte&(~(M_SAR<<3))) != 0xc6 )break;

		instr = (byte>>3)&M_SAR;
		num = code(dollar+1);
		sprintf(name,"%s\t0%02Xh",_SAR[instr],num);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* 8 bit arithmetic (IX\IY+offset) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SAR<<3))) != 0x86 )break;

		xx = (pre>>5)&M_SXX;
		offset = code(dollar+2);
		instr = (byte>>3)&M_SAR;
		sprintf(name,"%s\t(%s+0%02X)",_SAR[instr],_SXX[xx],offset);
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* INC\DEC register */
		byte = code(dollar);
		if( (byte&(~((M_SS<<3)|(M_SID)))) != 0x04 )break;

		instr = byte&M_SID;
		reg = (byte>>3)&M_SS;
		sprintf(name,"%s\t%s",_SID[instr],_Ss[reg]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* INC\DEC (IX\IY+offset) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xdd )break;
		byte = code(dollar+1);
		if( (byte&(~M_SID)) != 0x34 )break;

		xx = (pre>>5)&M_SXX;
		instr = byte&M_SID;
		offset = code(dollar+2);
		sprintf(name,"%s\t(%s+0%02Xh)",_SID[instr],_SXX[xx],offset);
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* CPI */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa1 )break;

		sprintf(name,"CPI");
		cont( dollar+2);
		return 2;
	}

	for(;;)
	{/* CPIR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb1 )break;

		sprintf(name,"CPIR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* CPD */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa9 )break;

		sprintf(name,"CPD");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* CPDR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb9 )break;

		sprintf(name,"CPDR");
		cont( dollar+2);
		return 2;
	}
	for(;;){/* DAA\CPL\SCF\CCF */
		byte = code(dollar);
		if( (byte&(~(M_SGC<<3))) != 0x27 )break;

		instr = (byte>>3)&M_SGC;
		sprintf(name,"%s",_SGC[instr]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* NOP */
		byte = code(dollar);
		if( byte != 0 )break;

		sprintf(name,"NOP");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* NEG */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0x44 )break;

		sprintf(name,"NEG");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* HALT */
		byte = code(dollar);
		if( byte != 0x76 )break;

		sprintf(name,"HALT");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* DI */
		byte = code(dollar);
		if( byte != 0xf3 )break;

		sprintf(name,"DI");
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* EI */
		byte = code(dollar);
		if( byte != 0xfb )break;

		sprintf(name,"EI");
		cont( dollar+1 );
		return 1;
	}

	for(;;)
	{/* IM 1\2 */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~8)) != 0x4e )break;

		num = (byte>>3)&01;
		sprintf(name,"IM\t%1d",num);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* IM 0 */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0x46 )break;

		sprintf(name,"IM\t%1d",0);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* rotate accu */
		byte = code(dollar);
		if( (byte&(~(M_SROTA<<3))) != 0x07 )break;

		instr = (byte>>3)&M_SROTA;
		sprintf(name,"%s",_SROTA[instr]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* rotete register */
		pre = code(dollar);
		if( pre != 0xcb ) break;
		byte = code(dollar+1);
		if( (byte&(~(M_SROT<<3)|M_SS)) != 0 )break;

		instr = (byte>>3)&M_SROT;
		reg = byte&M_SS;
		sprintf(name,"%s\t%s",_SROT[instr],_Ss[reg]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* rotate (IX\IY+offset) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		xx = (pre>>5)&M_SXX;
		pre = code(dollar+1);
		if( pre != 0xcb )break;
		byte = code(dollar+3);/*Not dollar+2,that is the offset */
		if( (byte&(~(M_SROT<<3))) != 6 )break;

		offset = code(dollar+2);
		instr = (byte>>3)&M_SROT;
		sprintf(name,"%s\t(%s+0%02Xh)",_SROT[instr],_SXX[xx],offset);
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* RLD */
		pre = code(dollar);
		if( pre != 0xcb )break;
		byte = code(dollar+1);
		if( byte != 0x6f )break;

		sprintf(name,"RLD");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* RRD */
		pre = code(dollar);
		if( pre != 0xcb )break;
		byte = code(dollar+1);
		if( byte != 0x67 )break;

		sprintf(name,"RRD");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* BIT\SET\RES */
		pre = code(dollar);
		if( pre != 0xcb )break;
		byte = code(dollar+1);
		if( (byte&(~((M_SBIT<<6)|(07<<3)|M_SS))) != 0x40 )break;

		reg = byte&M_SS;
		num = (byte>>3)&07;
		instr = (byte>>6)&M_SBIT;
		sprintf(name,"%s\t%1d,%s",_SBIT[instr],num,_Ss[reg]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* BIT\SET\RES (IX\IY+offset) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		xx = (pre>>5)&M_SXX;
		pre = code(dollar+1);
		if( pre != 0xcb )break;
		byte = code(dollar+3);/*Not dollar+2 because that is the offset*/
		if( (byte&(~((M_SBIT<<6)|(07<<3)))) != 0x46 )break;

		num = (byte>>3)&07;
		instr = (byte>>6)&M_SBIT;
		offset = code(dollar+2);
		sprintf(name,"%s\t%1d,(%s+0%02Xh)",_SBIT[instr],num,_SXX[xx],offset);
		cont(dollar+4);
		return 4;
	}
	for(;;)
	{/* JP cc,address*/
		byte = code(dollar);
		if( (byte&(~(M_SCE<<3))) != 0xc2 )break;

		cond = (byte>>3)&M_SCE;
		address = code(dollar+1) + 0x100*code(dollar+2);
		cont( address );
		sprintf(name,"JP\t%s,%s",_SCE[cond],label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* CALL cc,address*/
		byte = code(dollar);
		if( (byte&(~(M_SCE<<3))) != 0xc4 )break;

		cond = (byte>>3)&M_SCE;
		address = code(dollar+1) + 0x100*code(dollar+2);
		cont( address );
		sprintf(name,"CALL\t%s,%s",_SCE[cond],label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* RST */
		byte = code(dollar);
		if( (byte&(~(07<<3))) != 0xc7 )break;

		address = 8* ((byte>>3)&07);
		cont( address );
		sprintf(name,"RST\t0%02Xh",address);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* JP address */
		byte = code(dollar);
		if( byte != 0xc3 )break;

		address = code(dollar+1) + 0x100*code(dollar+2);
		cont( address );
		sprintf(name,"JP\t%s",label(address));
		return 3;
	}
	for(;;)
	{/* CALL address */
		byte = code(dollar);
		if( byte != 0xcd )break;

		address = code(dollar+1) + 0x100*code(dollar+2);
		cont( address );
		sprintf(name,"CALL\t%s",label(address));
		cont( dollar+3 );
		return 3;
	}
	for(;;)
	{/* JR cc,address */
		byte = code(dollar);
		if( (byte&(~(M_SCC<<3))) != 0x20 )break;

		cond = (byte>>3)&M_SCC;
		address = code(dollar+1);
		rcont( address , dollar +2 );
		sprintf(name,"JR\t%s,%s",_SCC[cond],rlabel(address,dollar+2));
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* JR address */
		byte = code(dollar);
		if( byte != 0x18 )break;

		address = code(dollar+1);
		rcont( address , dollar +2 );
		sprintf(name,"JR\t%s",rlabel(address,dollar+2));
		return 2;
	}
	for(;;)
	{/* JP (HL) */
		byte = code(dollar);
		if( byte != 0xe9 )break;
		sprintf(name,"JP\t(HL)");
		/* cont( HL ); Cont but where? */
		return 1;
	}
	for(;;)
	{/* JP (IX\IY) */
		pre = code(dollar);
		if( pre != 0xdd && pre != 0xfd )break;
		byte = code(dollar+1);
		if( byte != 0xe9 )break;

		xx = (pre>>5)&M_SXX;
		sprintf(name,"JP\t(%s)",_SXX[xx]);
		/* cont( I[XY] ); Cont but where ? */
		return 2;
	}
	for(;;)
	{/* DJNZ address */
		byte = code(dollar);
		if( byte != 0x10 )break;

		address = code(dollar+1);
		rcont( address , dollar +2 );
		sprintf(name,"DJNZ\t%s",rlabel(address,dollar+2));
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* RET */
		byte = code(dollar);
		if( byte != 0xc9 )break;

		sprintf(name,"RET");
		/* cont( dollar+1 ); RET ends a line*/
		return 1;
	}
	for(;;)
	{/* RET cc */
		byte = code(dollar);
		if( (byte&(~(M_SCE<<3))) != 0xc0 )break;

		cond = (byte>>3)&M_SCE;
		sprintf(name,"RET\t%s",_SCE[cond]);
		cont( dollar+1 );
		return 1;
	}
	for(;;)
	{/* RET I */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0x4d )break;

		sprintf(name,"RET\tI");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* RET N */
		pre = code(dollar);
		if( pre != 0xcb ) break;
		byte = code(dollar+1);
		if( byte != 0x45 )break;

		sprintf(name,"RET\tN");
		cont( dollar+2);
		return 2;
	}
	for(;;)/*???????????????????????????????????????????????????*/
	{/* IN num,(A) */
		byte = code(dollar);
		if( byte != 0xdb )break;

		num = code(dollar+1);
		sprintf(name,"IN\t(A),0%02Xh",num);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* IN (num),A */
		byte = code(dollar);
		if( byte != 0xd3 )break;

		num = code(dollar+1);
		sprintf(name,"IN\t(0%02Xh),A",num);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* IN reg,(C) */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SS<<3))) != 0x40 )break;

		reg = (byte>>3)&M_SS;
		sprintf(name,"IN\t%s,(C)",_Ss[reg]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* OUT (C),reg */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( (byte&(~(M_SS<<3))) != 0x41 )break;

		reg = (byte>>3)&M_SS;
		sprintf(name,"OUT\t(C),%s",_Ss[reg]);
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* INI */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa2 )break;

		sprintf(name,"INI");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* INIR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb2 )break;

		sprintf(name,"INIR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* OUTI */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xa3 )break;

		sprintf(name,"OUTI");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* OUTIR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xb3 )break;

		sprintf(name,"OUTIR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* IND */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xaa )break;

		sprintf(name,"IND");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* INDR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xba )break;

		sprintf(name,"INDR");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* OUTD */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xab )break;

		sprintf(name,"OUTD");
		cont( dollar+2);
		return 2;
	}
	for(;;)
	{/* OUTDR */
		pre = code(dollar);
		if( pre != 0xed )break;
		byte = code(dollar+1);
		if( byte != 0xbb )break;

		sprintf(name,"OUTDR");
		cont( dollar+2);
		return 2;
	}
	/* If no any instruction can fit the bytes. */
	return 0;
}/*End of dis. */
