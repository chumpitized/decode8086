#include <iostream>
#include <fstream>

using namespace std;

//reference manual: https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

enum Registers {
	ah, al, ax,
	bh, bl, bx,
	ch, cl, cx,
	dh, dl, dx,
	sp,
	bp,
	si,
	di
};

const char* register_strings[]{
	"ah", "al", "ax",
	"bh", "bl", "bx",
	"ch", "cl", "cx",
	"dh", "dl", "dx",
	"sp",
	"bp",
	"si",
	"di"
};

const char* determine_register(Registers reg1, Registers reg2, uint8_t w) {
	if (w == 0b00000001) return register_strings[reg2];
	else return register_strings[reg1];
}

const char* get_register(uint8_t byte, uint8_t w) {
	const char* reg;
	
	switch(byte) {
		case 0b00000000:
			reg = determine_register(Registers::al, Registers::ax, w);
			break;
		case 0b00000001:
			reg = determine_register(Registers::cl, Registers::cx, w);
			break;
		case 0b00000010:
			reg = determine_register(Registers::dl, Registers::dx, w);
			break;
		case 0b00000011:
			reg = determine_register(Registers::bl, Registers::bx, w);
			break;
		case 0b00000100:
			reg = determine_register(Registers::ah, Registers::sp, w);
			break;
		case 0b00000101:
			reg = determine_register(Registers::ch, Registers::bp, w);
			break;
		case 0b00000110:
			reg = determine_register(Registers::dh, Registers::si, w);
			break;
		case 0b00000111:
			reg = determine_register(Registers::bh, Registers::di, w);
			break;
	}
	return reg;
}

void decode_instruction(uint8_t byte1, uint8_t byte2) {
	//byte 1
	uint8_t opCode	= byte1 >> 2;
	uint8_t d 		= (byte1 & 0b00000010) >> 1;
	uint8_t w 		= byte1 & 0b00000001;

	//byte 2
	uint8_t mod		= byte2 >> 6;
	uint8_t reg		= (byte2 & 0b00111000) >> 3;
	uint8_t rm 		= byte2 & 0b00000111;

	const char* asmOpCode;

	switch(opCode) {
		case 0b00100010:
			asmOpCode = "mov";
			break;
		default:
			cout << "OPCODE WASN'T 100010" << endl;
			abort();
	}

	const char* src;
	const char* dst;

	if (d == 0b00000000) {
		src = get_register(reg, w);
		dst = get_register(rm, w);
	} else {
		src = get_register(rm, w);
		dst = get_register(reg, w);
	}

	cout << asmOpCode << " " << dst << ", " << src << endl;
}

int main() {
	fstream file;
    file.open("bin/listing_0038_many_register_mov", ios::in | ios::binary);
 
	if (file) {
 
		file.seekg(0, file.end);
		int length = file.tellg();

		//we actually need to support binary files of variable lengths
		if (length & 1) return 0;
		file.seekg(0, file.beg);

		//stack-allocated array
		uint8_t buffer[length];
 
		file.read(reinterpret_cast<char *>(buffer), length);
 
		file.close();
 
		for (int i = 0; i < length; i += 2) {
			uint8_t byte1 = buffer[i];
			uint8_t byte2 = buffer[i + 1];
			decode_instruction(byte1, byte2);
		}
 
	}

	//file.open("listing_0038_many_register_mov", ios::out | ios::binary);

    return 0;
}