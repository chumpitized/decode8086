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

const char* get_asm_op_code(uint8_t opcode) {
	switch(opcode) {
		case 0b10001000:
			return "mov";
			break;
		default:
			cout << "OPCODE WASN'T 100010" << endl;
			abort();
	}
}

const char* ea_calculation(uint8_t byte) {
	const char* rm;
	switch (byte) {
		case 0b00000000:
			rm = "bx + si";
			break;
		case 0b00000001:
			rm =  "bx + di";
			break;
		case 0b00000010:
			rm =  "bp + si";
			break;
		case 0b00000011:
			rm =  "bp + di";
			break;
		case 0b00000100:
			rm = "si";
			break;
		case 0b00000101:
			rm =  "di";
			break;
		case 0b00000110:
			rm = "bp";
			break;
		case 0b00000111:
			rm = "bx";
			break;
	}
	return rm;
}

int main() {
	fstream file;
    file.open("bin/listing_0038_many_register_mov", ios::in | ios::binary);
 
	if (file) {
 
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		//stack-allocated array
		uint8_t buffer[length];
 
		file.read(reinterpret_cast<char*>(buffer), length);
		file.close();

		//pointer in the byte buffer
		int ptr = 0;
		while (ptr < length) {
			uint8_t byte 	= buffer[ptr];
			//this is technically still wrong for all opcodes but will work for now
			uint8_t opcode 	= byte & 0b11111100;

			if (opcode == 0b10001000) {
				//register/memory to memory/register move

				uint8_t d = byte & 0b00000010;
				uint8_t w = byte & 0b00000001;

				ptr++;
				if (ptr >= length) {
					cout << "ERROR: TRIED PROCESSING INSTRUCTION BUT PTR PASSED FILE LENGTH" << endl;
					abort();
				}

				uint8_t byte2 	= buffer[ptr];
				uint8_t mod 	= byte2 & 0b11000000;
				uint8_t reg		= (byte2 & 0b00111000) >> 3; //we shift these bits to make it easier to match against both reg and rm
				uint8_t rm		= byte2 & 0b00000111;

				const char* asmCode = get_asm_op_code(opcode);

				switch (mod) {
					case 0b00000000:
						//16-bit displacement only when RM == 110
						break;

					case 0b01000000: {
						//8-bit displacement
						ptr++;
						//could check ptr against length after inc...
						uint8_t byte3 = buffer[ptr];
						const char* ea = ea_calculation(rm);
						const char* movRegister = get_register(reg, w);


						if (d == 0b00000010) {
							cout << asmCode << movRegister << ", [" << ea << " + " << byte3 << "]" << endl;  
						} else {
							cout << asmCode << " [" << ea << " + " << byte3 << "], " << movRegister << endl;
						}

						break;
					}

					case 0b10000000:
						//16-bit displacement
						// we need the 3rd bit, then the 4th
						ptr++;

						break;

					case 0b11000000: {
						//register mode

						const char* src;
						const char* dst;

						if (d == 0b00000000) {
							src = get_register(reg, w);
							dst = get_register(rm, w);
						} else {
							src = get_register(rm, w);
							dst = get_register(reg, w);
						}

						cout << asmCode  << " " << dst << ", " << src << endl;
						break;
					}
				}

				ptr++;
				continue;
			}

			if (opcode == 0b10110000) {
				//immediate to register move
				continue;
			}







		}

		//ultimately inc the ptr by 1
		//ptr++;
	}

    return 0;
}