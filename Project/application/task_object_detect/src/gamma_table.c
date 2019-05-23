#include "task_object_detect.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it =====================//
#if (APP_FACE_DETECTION_EN == 1)
//==================================================================================//

const unsigned char gamma_34[256] = {
 0, 39, 49, 56, 62, 67, 71, 75, 
 79, 82, 85, 88, 90, 93, 95, 97, 
 99, 102, 104, 105, 107, 109, 111, 113, 
 114, 116, 117, 119, 120, 122, 123, 125, 
 126, 127, 129, 130, 131, 132, 133, 135, 
 136, 137, 138, 139, 140, 141, 142, 143, 
 145, 146, 147, 148, 149, 149, 150, 151, 
 152, 153, 154, 155, 156, 157, 158, 159, 
 159, 160, 161, 162, 163, 164, 164, 165, 
 166, 167, 167, 168, 169, 170, 170, 171, 
 172, 173, 173, 174, 175, 176, 176, 177, 
 178, 178, 179, 180, 180, 181, 182, 182, 
 183, 184, 184, 185, 185, 186, 187, 187, 
 188, 189, 189, 190, 190, 191, 192, 192, 
 193, 193, 194, 195, 195, 196, 196, 197, 
 197, 198, 198, 199, 200, 200, 201, 201, 
 202, 202, 203, 203, 204, 204, 205, 205, 
 206, 206, 207, 207, 208, 208, 209, 209, 
 210, 210, 211, 211, 212, 212, 213, 213, 
 214, 214, 215, 215, 216, 216, 217, 217, 
 218, 218, 219, 219, 219, 220, 220, 221, 
 221, 222, 222, 223, 223, 223, 224, 224, 
 225, 225, 226, 226, 227, 227, 227, 228, 
 228, 229, 229, 229, 230, 230, 231, 231, 
 232, 232, 232, 233, 233, 234, 234, 234, 
 235, 235, 236, 236, 236, 237, 237, 238, 
 238, 238, 239, 239, 239, 240, 240, 241, 
 241, 241, 242, 242, 243, 243, 243, 244, 
 244, 244, 245, 245, 245, 246, 246, 247, 
 247, 247, 248, 248, 248, 249, 249, 249, 
 250, 250, 251, 251, 251, 252, 252, 252, 
 253, 253, 253, 254, 254, 254, 255, 255,
 };

const unsigned char gamma_30[256] = {
 0, 48, 60, 67, 73, 78, 83, 87, 
 90, 94, 97, 99, 102, 104, 107, 109, 
 111, 113, 115, 117, 119, 121, 122, 124, 
 125, 127, 129, 130, 131, 133, 134, 136, 
 137, 138, 139, 141, 142, 143, 144, 145, 
 146, 147, 148, 149, 151, 152, 153, 154, 
 155, 155, 156, 157, 158, 159, 160, 161, 
 162, 163, 164, 164, 165, 166, 167, 168, 
 168, 169, 170, 171, 172, 172, 173, 174, 
 174, 175, 176, 177, 177, 178, 179, 179, 
 180, 181, 181, 182, 183, 183, 184, 185, 
 185, 186, 187, 187, 188, 188, 189, 190, 
 190, 191, 191, 192, 193, 193, 194, 194, 
 195, 195, 196, 197, 197, 198, 198, 199, 
 199, 200, 200, 201, 201, 202, 202, 203, 
 203, 204, 204, 205, 205, 206, 206, 207, 
 207, 208, 208, 209, 209, 210, 210, 211, 
 211, 212, 212, 213, 213, 213, 214, 214, 
 215, 215, 216, 216, 217, 217, 217, 218, 
 218, 219, 219, 220, 220, 220, 221, 221, 
 222, 222, 223, 223, 223, 224, 224, 225, 
 225, 225, 226, 226, 227, 227, 227, 228, 
 228, 229, 229, 229, 230, 230, 230, 231, 
 231, 232, 232, 232, 233, 233, 233, 234, 
 234, 235, 235, 235, 236, 236, 236, 237, 
 237, 237, 238, 238, 238, 239, 239, 240, 
 240, 240, 241, 241, 241, 242, 242, 242, 
 243, 243, 243, 244, 244, 244, 245, 245, 
 245, 246, 246, 246, 247, 247, 247, 248, 
 248, 248, 249, 249, 249, 249, 250, 250, 
 250, 251, 251, 251, 252, 252, 252, 253, 
 253, 253, 253, 254, 254, 254, 255, 255,
 };


const unsigned char gamma_44[256] = {
 0, 22, 30, 36, 41, 45, 49, 52, 
 56, 59, 61, 64, 66, 69, 71, 73, 
 75, 77, 79, 81, 83, 85, 87, 88, 
 90, 92, 93, 95, 96, 98, 99, 101, 
 102, 104, 105, 106, 108, 109, 110, 112, 
 113, 114, 115, 117, 118, 119, 120, 121, 
 122, 123, 125, 126, 127, 128, 129, 130, 
 131, 132, 133, 134, 135, 136, 137, 138, 
 139, 140, 141, 142, 143, 143, 144, 145, 
 146, 147, 148, 149, 150, 151, 151, 152, 
 153, 154, 155, 156, 156, 157, 158, 159, 
 160, 160, 161, 162, 163, 164, 164, 165, 
 166, 167, 167, 168, 169, 170, 170, 171, 
 172, 173, 173, 174, 175, 175, 176, 177, 
 178, 178, 179, 180, 180, 181, 182, 182, 
 183, 184, 184, 185, 186, 186, 187, 188, 
 188, 189, 190, 190, 191, 191, 192, 193, 
 193, 194, 195, 195, 196, 196, 197, 198, 
 198, 199, 200, 200, 201, 201, 202, 202, 
 203, 204, 204, 205, 205, 206, 207, 207, 
 208, 208, 209, 209, 210, 211, 211, 212, 
 212, 213, 213, 214, 214, 215, 216, 216, 
 217, 217, 218, 218, 219, 219, 220, 220, 
 221, 221, 222, 222, 223, 224, 224, 225, 
 225, 226, 226, 227, 227, 228, 228, 229, 
 229, 230, 230, 231, 231, 232, 232, 233, 
 233, 234, 234, 235, 235, 236, 236, 237, 
 237, 238, 238, 238, 239, 239, 240, 240, 
 241, 241, 242, 242, 243, 243, 244, 244, 
 245, 245, 246, 246, 246, 247, 247, 248, 
 248, 249, 249, 250, 250, 251, 251, 251, 
 252, 252, 253, 253, 254, 254, 255, 255,
 };


const unsigned char gamma_58[256] = {
 0, 10, 15, 19, 23, 26, 29, 32, 
 34, 37, 39, 41, 43, 45, 47, 49, 
 51, 53, 55, 57, 58, 60, 62, 63, 
 65, 66, 68, 69, 71, 72, 74, 75, 
 77, 78, 79, 81, 82, 83, 85, 86, 
 87, 88, 90, 91, 92, 93, 94, 96, 
 97, 98, 99, 100, 101, 103, 104, 105, 
 106, 107, 108, 109, 110, 111, 112, 113, 
 114, 115, 116, 117, 118, 119, 120, 121, 
 122, 123, 124, 125, 126, 127, 128, 129, 
 130, 131, 132, 133, 134, 135, 136, 137, 
 138, 138, 139, 140, 141, 142, 143, 144, 
 145, 146, 146, 147, 148, 149, 150, 151, 
 152, 152, 153, 154, 155, 156, 157, 157, 
 158, 159, 160, 161, 161, 162, 163, 164, 
 165, 165, 166, 167, 168, 169, 169, 170, 
 171, 172, 173, 173, 174, 175, 176, 176, 
 177, 178, 179, 179, 180, 181, 182, 182, 
 183, 184, 185, 185, 186, 187, 187, 188, 
 189, 190, 190, 191, 192, 192, 193, 194, 
 195, 195, 196, 197, 197, 198, 199, 199, 
 200, 201, 202, 202, 203, 204, 204, 205, 
 206, 206, 207, 208, 208, 209, 210, 210, 
 211, 212, 212, 213, 214, 214, 215, 216, 
 216, 217, 218, 218, 219, 220, 220, 221, 
 221, 222, 223, 223, 224, 225, 225, 226, 
 227, 227, 228, 228, 229, 230, 230, 231, 
 232, 232, 233, 233, 234, 235, 235, 236, 
 237, 237, 238, 238, 239, 240, 240, 241, 
 241, 242, 243, 243, 244, 244, 245, 246, 
 246, 247, 247, 248, 249, 249, 250, 250, 
 251, 252, 252, 253, 253, 254, 254, 255,
 };
 #endif


