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
		case 0b10110000:
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
			rm = "di";
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
    file.open("bin/listing_0039_more_movs", ios::in | ios::binary);
 
	if (file) {
 
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		//stack-allocated array
		uint8_t buffer[length];
 
		file.read(reinterpret_cast<char*>(buffer), length);
		file.close();

		//idx in the byte buffer
		int idx = 0;
		while (idx < length) {
			uint8_t byte 	= buffer[idx];


			// Register/memory to/from register
			if ((byte & 0b11111100) == 0b10001000) {
				//register/memory to memory/register move

				uint8_t d = byte & 0b00000010;
				uint8_t w = byte & 0b00000001;

				idx++;
				if (idx >= length) {
					cout << "ERROR: TRIED PROCESSING INSTRUCTION BUT idx PASSED FILE LENGTH" << endl;
					abort();
				}

				uint8_t byte2 	= buffer[idx];
				uint8_t mod 	= byte2 & 0b11000000;
				uint8_t reg		= (byte2 & 0b00111000) >> 3; //we shift these bits to make it easier to match against both reg and rm
				uint8_t rm		= byte2 & 0b00000111;

				const char* asmCode = get_asm_op_code(0b10001000);

				switch (mod) {
					//No displacement, except when RM == 110 (then 16-bit disp.)
					case 0b00000000: {
						const char* ea = ea_calculation(rm);
						const char* movRegister = get_register(reg, w);
					
						//do 16-bit displacement
						if (rm == 0b00000110) {
							idx++;
							uint8_t byte3 = buffer[idx];
							idx++;
							uint8_t byte4 = buffer[idx];

							uint16_t disp = (uint16_t)byte4 >> 8 | byte3;

							if (d == 0b00000010) {
								cout << asmCode << " " << movRegister << ", [" << ea << " + " << +disp << "]" << endl;
							} else {
								cout << asmCode << " [" << ea << " + " << +disp << "], " << movRegister << endl;
							}					

							break;
						}

						//else no displacement		

						if (d == 0b00000010) {
							cout << asmCode << " " <<  movRegister << ", [" << ea << "]" << endl;
						} else {
							cout << asmCode << " [" << ea << "], " << movRegister << endl;
						}

						break;
					}

					//8-bit displacement
					case 0b01000000: {
						idx++;
						uint8_t byte3 = buffer[idx];

						const char* ea = ea_calculation(rm);
						const char* movRegister = get_register(reg, w);

						if (d == 0b00000010) {
							cout << asmCode << " " << movRegister << ", [" << ea << " + " << +byte3 << "]" << endl;  
						} else {
							cout << asmCode << " [" << ea << " + " << +byte3 << "], " << movRegister << endl;
						}

						break;
					}

					//16-bit displacement
					case 0b10000000: {					
						idx++;
						uint8_t byte3 = buffer[idx];
						idx++;
						uint8_t byte4 = buffer[idx];

						uint16_t disp = (uint16_t)byte4 << 8 | byte3;

						const char* ea = ea_calculation(rm);
						const char* movRegister = get_register(reg, w);		

						if (d == 0b00000010) {							
							cout << asmCode << " " << movRegister << ", [" << ea << " + " << +disp << "]" << endl;  
						} else {
							cout << asmCode << " [" << ea << " + " << +disp << "], " << movRegister << endl;
						}					

						break;
					}

					//register mode
					case 0b11000000: {
						const char* src;
						const char* dst;

						if (d == 0b00000000) {
							src = get_register(reg, w);
							dst = get_register(rm, w);
						} else {
							src = get_register(rm, w);
							dst = get_register(reg, w);
						}

						cout << asmCode << " " << dst << ", " << src << endl;
						break;
					}
				}

				idx++;
				continue;
			}



			//immediate to register move
			if ((byte & 0b11110000) == 0b10110000) {
				uint8_t w 	= (byte & 0b00001000) >> 3;
				uint8_t reg = byte & 0b00000111;

				const char* asmCode =  get_asm_op_code(0b10110000);

				const char* dst = get_register(reg, w);

				idx++;
				uint8_t byte2 = buffer[idx];

				if (w == 0b0000001) {
					idx++;
					uint8_t byte3 = buffer[idx];
					uint16_t disp = (uint16_t)byte3 << 8 | byte2;

					cout << asmCode << " " << dst << ", " << disp << endl;
					
					idx++;
					continue;
				}

				cout << asmCode << " " << dst << ", " << +byte2 << endl;

				idx++;
				continue;
			}

			//idx++;
		}
	}

    return 0;
}