#include	"sprite.h"
const INT16U ALIGN4 M0_SPRITE_Palette1[]={
	0x801F, 0x7B1F, 0x7B3F, 0x7B7F, 0x7F9F, 0x7FBF, 0x7FFF, 0x7B7F, 0x7F9F, 0x7912, 0x7B3F, 0x7953, 0x7B7F, 0x7DF6, 0x7FBF, 0x7B5F,
	0x0000, 0x6739, 0x5F9F, 0x7FFF, 0x6739, 0x0C63, 0x7E79, 0x78D0, 0x7B5F, 0x6F7B, 0x79D5, 0x7B3F, 0x63BF, 0x7FDF, 0x6B5A, 0x7B3F,
	0x73FF, 0x7FDF, 0x6318, 0x7F1B, 0x6FDF, 0x7F9F, 0x1063, 0x14A6, 0x7E58, 0x6F7B, 0x7932, 0x6EDC, 0x5294, 0x4E73, 0x5EF7, 0x4E73,
	0x6B5A, 0x7E17, 0x7FBF, 0x6BBF, 0x739C, 0x7DB5, 0x7933, 0x7B7F, 0x771F, 0x56B5, 0x7B5F, 0x5AD6, 0x7EDA, 0x1084, 0x771E, 0x1CE7,

	0x4210, 0x35AD, 0x4A52, 0x2529, 0x6318, 0x72FD, 0x7EB9, 0x294A, 0x7FFF, 0x318C, 0x6F7B, 0x7DF6, 0x7E78, 0x4210, 0x7BFF, 0x4631,
	0x2529, 0x39CE, 0x7BDE, 0x7F5C, 0x7973, 0x7BDE, 0x2D6B, 0x7B3F, 0x5B7E, 0x7FFF, 0x77BD, 0x7FFF, 0x7D94, 0x77BD, 0x739C, 0x5656,
	0x7974, 0x294A, 0x6739, 0x2108, 0x66FA, 0x7F3C, 0x7FFF, 0x6739, 0x318C, 0x77BD, 0x7E37, 0x49F3, 0x7BDE, 0x67DF, 0x7D11, 0x7FFF,
	0x5E77, 0x7FFF, 0x7DB5, 0x6ABB, 0x7EDA, 0x7B5F, 0x7FDE, 0x5235, 0x48EB, 0x7BFF, 0x7FDF, 0x5DB2, 0x7B9F, 0x3635, 0x4697, 0x7FFF,

	0x52F8, 0x2866, 0x733D, 0x6AFB, 0x7FBE, 0x7B5F, 0x14E9, 0x773E, 0x7F9D, 0x739C, 0x7E57, 0x66BA, 0x75D4, 0x7B7F, 0x552E, 0x7FFF,
	0x6F1C, 0x775E, 0x7A78, 0x7F9E, 0x5676, 0x7994, 0x296C, 0x34C8, 0x4B3E, 0x61D3, 0x1422, 0x6279, 0x1864, 0x6951, 0x7F7D, 0x7F5D,
	0x610E, 0x739C, 0x779E, 0x6255, 0x62D9, 0x7D73, 0x6B1A, 0x775E, 0x5E97, 0x6299, 0x537F, 0x7110, 0x735D, 0x779D, 0x416D, 0x573B,
	0x6F5C, 0x6719, 0x68AC, 0x71B3, 0x7FFE, 0x31F1, 0x4652, 0x6E15, 0x5A33, 0x354B, 0x55B0, 0x4E11, 0x771A, 0x777E, 0x7BBE, 0x7BBF,

	0x7FDF, 0x7FFF, 0x2929, 0x737D, 0x5F9E, 0x7FBF, 0x214B, 0x6B3B, 0x7FBF, 0x66F8, 0x675B, 0x6B1B, 0x4468, 0x190A, 0x777E, 0x7636,
	0x2D2A, 0x4AD9, 0x7D52, 0x42FC, 0x7151, 0x7B9E, 0x2DAF, 0x4F1B, 0x7532, 0x7572, 0x298E, 0x6676, 0x6F9D, 0x73BE, 0x7B7F, 0x7B7F,
	0x52D7, 0x7B7F, 0x7D31, 0x35CF, 0x6E97, 0x7B1E, 0x7698, 0x7ADC, 0x6AF8, 0x7952, 0x679D, 0x75F6, 0x7172, 0x7B7F, 0x75B6, 0x6B7C,
	0x7ABB, 0x633A, 0x7256, 0x7A79, 0x6F19, 0x7639, 0x76BB, 0x7A9A, 0x7B3E, 0x633A, 0x7F7E, 0x7A59, 0x6EB8, 0x767A, 0x7B5F, 0x7B5F,



	0x801F, 0x7FFF, 0x7DF6, 0x7C0F, 0x7E9A, 0x7E59, 0x7FFF, 0x7FF8, 0x7E99, 0x1063, 0x18C7, 0x7F1C, 0x1CE7, 0x4210, 0x7FFA, 0x3DEF,
	0x2D6B, 0x7F3C, 0x2108, 0x7BDE, 0x31AC, 0x53BF, 0x4545, 0x7BDE, 0x4E73, 0x77DD, 0x51C7, 0x7CF2, 0x7D13, 0x2445, 0x2529, 0x7FFE,
	0x7E79, 0x7FFC, 0x7FFF, 0x28C8, 0x4631, 0x4987, 0x7E17, 0x4E73, 0x7FFF, 0x7FFF, 0x6318, 0x318C, 0x7EBA, 0x7EFC, 0x5294, 0x7F3D,
	0x5FDF, 0x55D1, 0x5AD6, 0x59E8, 0x7FFF, 0x35AD, 0x77BD, 0x7D99, 0x39CE, 0x294A, 0x548B, 0x7EDB, 0x739C, 0x3867, 0x73FF, 0x34CA,

	0x7FF7, 0x2D0A, 0x6739, 0x7EBB, 0x74F1, 0x410D, 0x312A, 0x5EF7, 0x7F5D, 0x56B5, 0x57DF, 0x7B9F, 0x7FEB, 0x4D8F, 0x6B5A, 0x6B5A,
	0x4A52, 0x6F7B, 0x739C, 0x7F9E, 0x6ABF, 0x416E, 0x4595, 0x7FF1, 0x392C, 0x5EF7, 0x6BFF, 0x7FDF, 0x21AF, 0x64AE, 0x77BD, 0x7EBA,
	0x7E7A, 0x7FBA, 0x4469, 0x5212, 0x5AF6, 0x3905, 0x5F9F, 0x5A2B, 0x7F99, 0x6B5A, 0x7FE3, 0x7D15, 0x49AA, 0x2D2E, 0x5AD6, 0x6738,
	0x739D, 0x739C, 0x69F5, 0x76FF, 0x4A52, 0x6F7B, 0x7BFE, 0x7DB6, 0x3D24, 0x7257, 0x30EE, 0x63BF, 0x6318, 0x4168, 0x4DEC, 0x2E53,

	0x63DF, 0x7D95, 0x7E39, 0x56B5, 0x6EB8, 0x6BBF, 0x4FBF, 0x24EC, 0x6739, 0x666C, 0x7757, 0x5E34, 0x6E35, 0x6739, 0x36E7, 0x6318,
	0x7F3E, 0x7472, 0x73BC, 0x5ED6, 0x7B7F, 0x7554, 0x7FFF, 0x4B5D, 0x51D9, 0x50ED, 0x77FF, 0x7F5E, 0x2909, 0x5A3D, 0x4E0F, 0x73BC,
	0x7D56, 0x737B, 0x665E, 0x66D5, 0x72FA, 0x6931, 0x5570, 0x65B3, 0x6F7B, 0x4B39, 0x6B5A, 0x7AF4, 0x7F1D, 0x73BF, 0x6B7B, 0x7996,
	0x5E70, 0x3AD7, 0x49F0, 0x771F, 0x77DF, 0x76FB, 0x7A9A, 0x7A58, 0x1D2A, 0x7E1B, 0x216C, 0x3B0B, 0x3E96, 0x675A, 0x58F0, 0x7FFF,

	0x539D, 0x6F5B, 0x3952, 0x3DCD, 0x4752, 0x5BBD, 0x154B, 0x7ABA, 0x7EBB, 0x335B, 0x7699, 0x7A5E, 0x7FFE, 0x3E8E, 0x6110, 0x3D8D,
	0x5B7D, 0x6EB1, 0x7A78, 0x7E78, 0x4AD6, 0x4ED8, 0x5EB4, 0x6296, 0x7ABF, 0x5E7B, 0x3D32, 0x4955, 0x5B3A, 0x76BA, 0x5A93, 0x5F98,
	0x733B, 0x7659, 0x7637, 0x6F16, 0x6F19, 0x4A32, 0x5F5A, 0x7A79, 0x2D8B, 0x49F7, 0x6B6F, 0x6DD7, 0x77BC, 0x7E9C, 0x679C, 0x7B3C,
	0x31CD, 0x5318, 0x6EFD, 0x72DA, 0x76B8, 0x6738, 0x72BF, 0x7EE0, 0x6BDE, 0x7AFC, 0x7703, 0x6F1E, 0x671C, 0x0FBF, 0x6B3C, 0x7712
};