	# for IPUV
	# input shift: 4
	# x-counter shift: 4
	# out counter shift 3 (x4)
# function:
# compress RGB according to the compression factor
# in0  :  R
# in1  :  G
# in2  :  B
# in3  :  comression factor in (8.8)
# out0 :  B compressed, G compressed, R compressed

        
.global rgb_compress_start
rgb_compress_start:
  mov confalu,(0 /*unsigned*/ | (1<<1) /*saturate*/ | (8<<4) /*shr*/)
  done rgb_compress_loop,i

rgb_compress_loop:	

  mulh vh4 ,vh0, vh15
  mulh vh9 ,vh5, vh15
  mulh vh14,vh10,vh15

rgb_compress_out0:	
  dvot vv7,rgb_compress_out1,o
rgb_compress_out1:	
  dvot vv6,rgb_compress_out2,o
rgb_compress_out2:	
  dvot vv5,rgb_compress_out3,o
rgb_compress_out3:	
  dvot vv4,rgb_compress_loop,ixo
	

  //halt
.global rgb_compress_end
rgb_compress_end:
