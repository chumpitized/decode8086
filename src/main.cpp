#include <iostream>
#include <fstream>

using namespace std;

typedef uint8_t u8;
typedef uint16_t u16;


uint16_t FLAGS = 0;

//reference manual: https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

uint16_t registers[8] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

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

uint16_t register_indexes[] {
	0, 0, 0,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	4,
	5,
	6,
	7
};

const char* register_names[]{
	"ah", "al", "ax",
	"bh", "bl", "bx",
	"ch", "cl", "cx",
	"dh", "dl", "dx",
	"sp",
	"bp",
	"si",
	"di"
};

//used by reg and rm values (0-7)
const char* effective_addresses[] {
	"bx + si",
	"bx + di",
	"bp + si",
	"bp + di",
	"si",
	"di",
	"bp",
	"bx"
};

inline uint16_t determine_register(Registers reg1, Registers reg2, uint8_t w) {
	return w == 1 ? reg2 : reg1;
}

uint16_t get_register_indexes(uint8_t byte, uint8_t w) {
	switch (byte) {
		case 0b00000000: return determine_register(Registers::al, Registers::ax, w);
		case 0b00000001: return determine_register(Registers::cl, Registers::cx, w);
		case 0b00000010: return determine_register(Registers::dl, Registers::dx, w);
		case 0b00000011: return determine_register(Registers::bl, Registers::bx, w);
		case 0b00000100: return determine_register(Registers::ah, Registers::sp, w);
		case 0b00000101: return determine_register(Registers::ch, Registers::bp, w);
		case 0b00000110: return determine_register(Registers::dh, Registers::si, w);
		case 0b00000111: return determine_register(Registers::bh, Registers::di, w);
		default: return 0;
	}
}

const char* get_asm_op_code(uint8_t opcode) {
	switch(opcode) {
		case 0b01110100:
			return "je";
		case 0b01111100:
			return "jl";
		case 0b01111110:
			return "jle";
		case 0b01110010:
			return "jb";
		case 0b01110110:
			return "jbe";
		case 0b01111010:
			return "jp";
		case 0b01110000:
			return "jo";
		case 0b01111000:
			return "js";
		case 0b01110101:
			return "jne";
		case 0b01111101:
			return "jnl";
		case 0b01111111:
			return "jnle";
		case 0b01110011:
			return "jnb";
		case 0b01110111:
			return "jnbe";
		case 0b01111011:
			return "jnp";
		case 0b01110001:
			return "jno";
		case 0b01111001:
			return "jns";
		case 0b11100010:
			return "loop";
		case 0b11100001:
			return "loopz";
		case 0b11100000:
			return "loopnz";
		case 0b11100011:
			return "jcxz";
		case 0b10001000:
			return "mov";
		case 0b10110000:
			return "mov";
		default:
			cout << "OPCODE NOT FOUND" << endl;
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
    file.open("bin/listing_0044_register_movs", ios::in | ios::binary);
 
	if (file) {
		file.seekg(0, file.end);
		int length = file.tellg();
		file.seekg(0, file.beg);

		uint8_t buffer[length];
 
		file.read(reinterpret_cast<char*>(buffer), length);
		file.close();

		int idx = 0;
		while (idx < length) {
			uint8_t byte 					= buffer[idx];
			uint8_t mov_add_sub_cmp_code 	= 0b11111100 & byte;
			uint8_t imm_mov_code			= 0b11110000 & byte;
			uint8_t jump_code				= 0b11111111 & byte;

			if (
				jump_code == 0b01110100 ||
				jump_code == 0b01111100 ||
				jump_code == 0b01111110 ||
				jump_code == 0b01110010 ||
				jump_code == 0b01110110 ||
				jump_code == 0b01111010 ||
				jump_code == 0b01110000 ||
				jump_code == 0b01111000 ||
				jump_code == 0b01110101 ||
				jump_code == 0b01111101 ||
				jump_code == 0b01111111 ||
				jump_code == 0b01110011 ||
				jump_code == 0b01110111 ||
				jump_code == 0b01111011 ||
				jump_code == 0b01110001 ||
				jump_code == 0b01111001 ||
				jump_code == 0b11100010 ||
				jump_code == 0b11100001 ||
				jump_code == 0b11100000 ||
				jump_code == 0b11100011
			) {
				cout << get_asm_op_code(byte) << " ";

				idx++;
				uint8_t byte2 = buffer[idx];

				cout << +byte2 << endl;

				idx++;
				continue;
			}

			if (mov_add_sub_cmp_code == 0b00000100 || mov_add_sub_cmp_code == 0b00101100 || mov_add_sub_cmp_code == 0b00111100) {
				uint8_t w = byte & 0b00000001;

				idx++;
				uint8_t byte2 = buffer[idx];

				if (mov_add_sub_cmp_code == 0b00000100) cout << "add ";
				if (mov_add_sub_cmp_code == 0b00101100) cout << "sub ";
				if (mov_add_sub_cmp_code == 0b00111100) cout << "cmp ";

				if (w == 1) {
					idx++;
					uint8_t byte3 = buffer[idx];

					uint16_t data = (uint16_t)byte3 << 8 | byte2;

					cout << "ax, " << +data << endl;
				} else {
					cout << "al, " << +byte2 << endl;
				}

				idx++;
				continue;
			}

			//add, sub, cmp
			if (mov_add_sub_cmp_code == 0b00000000 || mov_add_sub_cmp_code == 0b00101000 || mov_add_sub_cmp_code == 0b00111000) {
				idx++;
				uint8_t byte2 = buffer[idx];
				uint8_t mod = (byte2 & 0b11000000) >> 6;
				uint8_t reg = (byte2 & 0b00111000) >> 3;
				uint8_t rm	= byte2 & 0b00000111;
				
				uint8_t d = (byte & 0b00000010) >> 1;
				uint8_t w = byte & 0b00000001;

				const char* asmOpCode;

				if (mov_add_sub_cmp_code == 0b00000000) asmOpCode = "add ";
				if (mov_add_sub_cmp_code == 0b00101000) asmOpCode = "sub ";
				if (mov_add_sub_cmp_code == 0b00111000) asmOpCode = "cmp ";

				cout << asmOpCode;

				switch (mod) {
					case 0b00000000: {
						const char* ea 		= ea_calculation(rm);
						uint16_t regIdx 	= get_register_indexes(reg, w);
						const char* regName = register_names[regIdx];

						const char* dest;
						const char* src;

						if (d == 1) {
							cout << regName << ", " << "[" << ea << "]";
						} else {
							cout << "[" << ea << "], " << regName;
						}
						cout << endl;

						break;
					}

					case 0b00000001: {
						const char* ea 		= ea_calculation(rm);
						uint16_t regIdx 	= get_register_indexes(reg, w);
						const char* regName = register_names[regIdx];

						idx++;
						uint8_t disp = buffer[idx];

						if (d == 1) {
							cout << regName << ", " << "[" << ea << " + " <<  +disp << "]" << endl;
						} else {
							cout << "[" << ea << " + " <<  +disp << "], " << regName << endl;
						}

						break;
					}

					case 0b00000010: {
						const char* ea 		= ea_calculation(rm);
						uint16_t regIdx 	= get_register_indexes(reg, w);
						const char* regName = register_names[regIdx];

						const char* dest;
						const char* src;

						idx++;
						uint8_t byte3 = buffer[idx];
						idx++;
						uint8_t byte4 = buffer[idx];

						uint16_t disp = (uint16_t)byte4 << 8 | byte3;

						if (d == 1) {
							cout << regName << ", " << "[" << ea << " + " <<  +disp << "]" << endl;;
						} else {
							cout << "[" << ea << " + " <<  +disp << "], " << regName << endl;
						}
						
						break;
					}

					case 0b00000011: {						
						const char* src;
						const char* dst;

						//16-bit displacement
						if (rm == 0b00000110) {
							idx++;
							uint8_t byte3 = buffer[idx];
							idx++;
							uint8_t byte4 = buffer[idx];
							
							uint16_t disp = (uint16_t)byte4 << 8 | byte3;

							if (d == 1) {
								uint16_t regIdx = get_register_indexes(reg, w);

								src = ea_calculation(rm);
								dst = register_names[regIdx];

								cout << dst << ", [" << src << " + " << +disp << "]" << endl;
							} else {
								uint16_t regIdx = get_register_indexes(rm, w);

								src = register_names[regIdx];
								dst = ea_calculation(rm);

								cout << "[" << dst << " + " << +disp << "], " << src << endl;
							}
						}
						
						else {
							if (d == 1) {
								uint16_t reg1Idx = get_register_indexes(rm, w);
								uint16_t reg2Idx = get_register_indexes(reg, w);

								src = register_names[reg1Idx];
								dst = register_names[reg2Idx];
							} else {
								uint16_t reg1Idx = get_register_indexes(reg, w);
								uint16_t reg2Idx = get_register_indexes(rm, w);

								src = register_names[reg1Idx];
								dst = register_names[reg2Idx];
							}

							cout << dst << ", " << src << endl;

						}

						break;
					}
				}

				idx++;
				continue;
			}

			//CMP may need special handling for sign extension bit...
			//ADD, SUB, CMP immediate-to-register
			if (mov_add_sub_cmp_code == 0b10000000) {
				idx++;
				uint8_t byte2 			= buffer[idx];
				uint8_t mod 			= (byte2 & 0b11000000) >> 6;
				uint8_t w 				= (byte & 0b00000001);
				uint8_t s 				= (byte & 0b00000010) >> 1;
				uint8_t rm 				= byte2 & 0b00000111;
				const char* asmOpCode;

				if ((byte2 & 0b00111000) == 0b00000000) asmOpCode = "add ";
				if ((byte2 & 0b00111000) == 0b00101000) asmOpCode = "sub ";
				if ((byte2 & 0b00111000) == 0b00111000) asmOpCode = "cmp ";
				
				cout << asmOpCode;

				switch (mod) {
					case 0b00000000: {
						const char* ea = ea_calculation(rm);
						cout << "[" << ea << "], ";

						idx++;
						uint8_t data = buffer[idx];

						if (s == 0 && w == 1) {
							idx++;
							uint8_t data2 = buffer[idx];

							uint16_t allData = data2 << 8 | data;

							cout << +allData;
						} else {
							cout << +data;
						}
						 
						cout << endl;

						break;
					}

					case 0b00000001: {
						const char* ea = ea_calculation(rm);

						idx++;
						uint8_t byte3 = buffer[idx];

						cout << "[" << ea << " + " << +byte3 << "], ";

						idx++;
						uint8_t data = buffer[idx];

						if (s == 0 && w == 1) {
							idx++;
							uint8_t data2 = buffer[idx];

							uint16_t allData = data2 << 8 | data;

							cout << +allData;
						} else {
							cout << +data;
						}
						cout << endl;

						break;
					}

					case 0b00000010: {
						const char* ea = ea_calculation(rm);
						
						idx++;
						uint8_t byte3 = buffer[idx];
						idx++;
						uint8_t byte4 = buffer[idx];

						uint16_t disp = (uint16_t)byte4 << 8 | byte3;

						cout << "[" << ea << " + " << +disp << "], ";

						idx++;
						uint8_t data = buffer[idx];

						if (s == 0 && w == 1) {
							idx++;
							uint8_t data2 = buffer[idx];

							uint16_t allData = data2 << 8 | data;

							cout << +allData;
						} else {
							cout << +data;
						}
						cout << endl;

						break;
					}

					case 0b00000011: {
						uint16_t regIdx 	= get_register_indexes(rm, w);
						const char* regName = register_names[regIdx];

						cout << regName << ", "; 

						idx++;
						uint8_t data = buffer[idx];

						if (s == 0 && w == 1) {
							idx++;
							uint8_t data2 = buffer[idx];

							uint16_t allData = data2 << 8 | data;

							cout << +allData;
						} else {
							cout << +data;
						}
						cout << endl;

						break;
					}
				}

				idx++;
				continue;
			}

			// Mov Register/memory to/from register
			if (mov_add_sub_cmp_code == 0b10001000) {
				uint8_t d = byte & 0b00000010;
				uint8_t w = byte & 0b00000001;

				idx++;
				uint8_t byte2 	= buffer[idx];
				uint8_t mod 	= byte2 & 0b11000000;
				uint8_t reg		= (byte2 & 0b00111000) >> 3;
				uint8_t rm		= byte2 & 0b00000111;

				const char* asmCode = get_asm_op_code(0b10001000);

				switch (mod) {
					//No displacement, except when RM == 110 (then 16-bit disp.)
					case 0b00000000: {
						const char* ea 			= ea_calculation(rm);
						uint16_t regIdx 		= get_register_indexes(reg, w);
						const char* movRegister = register_names[regIdx];
					
						//do 16-bit displacement
						if (rm == 0b00000110) {
							idx++;
							uint8_t byte3 = buffer[idx];
							idx++;
							uint8_t byte4 = buffer[idx];

							uint16_t disp = (uint16_t)byte4 << 8 | byte3;

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
						uint16_t regIdx = get_register_indexes(reg, w);
						const char* movRegister = register_names[regIdx];

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

						const char* ea 			= ea_calculation(rm);
						uint16_t regIdx 		= get_register_indexes(reg, w);
						const char* movRegister = register_names[regIdx];		

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
						uint16_t srcIndex;
						uint16_t dstIndex;

						if (d == 0b00000000) {
							srcIndex 	= get_register_indexes(reg, w);
							src 		= register_names[srcIndex];
	
							dstIndex	= get_register_indexes(rm, w);
							dst 		= register_names[dstIndex];
						} else {
							srcIndex 	= get_register_indexes(rm, w);
							src 		= register_names[srcIndex];
							
							dstIndex	= get_register_indexes(reg, w);
							dst 		= register_names[dstIndex];
						}

						//perform the actual move
						registers[register_indexes[dstIndex]] = registers[register_indexes[srcIndex]];

						//print the asm instruction
						cout << asmCode << " " << dst << ", " << src << endl;
						break;
					}
				}

				idx++;
				continue;
			}

			//immediate to register move
			if (imm_mov_code == 0b10110000) {
				uint8_t w 	= (byte & 0b00001000) >> 3;
				uint8_t reg = byte & 0b00000111;

				const char* asmCode =  get_asm_op_code(0b10110000);

				uint16_t regIdx 	= get_register_indexes(reg, w);
				uint16_t regAddr	= register_indexes[regIdx];
				const char* dst 	= register_names[regIdx];

				idx++;
				uint8_t byte2 = buffer[idx];

				if (w == 0b00000001) {
					idx++;
					uint8_t byte3 = buffer[idx];
					uint16_t disp = (uint16_t)byte3 << 8 | byte2;

					registers[regAddr] = disp;

					cout << asmCode << " " << dst << ", " << disp << endl;
				} else {
					cout << asmCode << " " << dst << ", " << +byte2 << endl;
				}				

				idx++;
				continue;
			}

			idx++;
		}
	}

	for (int i = 0; i < 8; ++i) {
		cout << i << " : " << registers[i] << endl;
	}

    return 0;
}