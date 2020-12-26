import os
import time
test_data_root_path = "./test_data/"
for test_file in os.listdir(test_data_root_path):
    if (not "ans" in test_file) and not ("seg" in test_file):
        clear_file_name = test_file.replace(".txt", "")
        seg_file_name = f"{clear_file_name}_seg.txt"
        convert = f"perl separator_big5.pl ./{test_data_root_path}/{test_file} > ./{test_data_root_path}/{seg_file_name}"
        os.system(convert)

        # ori bi
        srilm_bi_call = f"disambig -text ./test_data/{seg_file_name} -map ./ZhuYin-Big5.map -lm bigram.lm -order 2 > ./out_data/{clear_file_name}_srilm_bi.txt"
        start_time = time.time()
        os.system(srilm_bi_call)
        print(f"[{clear_file_name} || SRILM || BI]--- {(time.time() - start_time):.4f} seconds ---")

        # ori tri
        srilm_tri_call = f"disambig -text ./test_data/{seg_file_name} -map ./ZhuYin-Big5.map -lm trigram.lm -order 3 > ./out_data/{clear_file_name}_srilm_tri.txt"
        start_time = time.time()
        os.system(srilm_tri_call)
        print(f"[{clear_file_name} || SRILM || TRI]--- {(time.time() - start_time):.4f} seconds ---")
        
        # my bi 
        my_bi_call = f"./mydisambig ./test_data/{seg_file_name} ./ZhuYin-Big5.map bigram.lm ./out_data/{clear_file_name}_my_bi.txt"
        start_time = time.time()
        os.system(my_bi_call)
        print(f"[{clear_file_name} || MY || BI ]--- {(time.time() - start_time):.4f} seconds ---")
        
        # by tri
        my_tri_call = f"./mydisambig ./test_data/{seg_file_name} ./ZhuYin-Big5.map trigram.lm ./out_data/{clear_file_name}_my_tri.txt 3"
        start_time = time.time()
        os.system(my_tri_call)
        print(f"[{clear_file_name} || MY || TRI ]--- {(time.time() - start_time):.4f} seconds ---")