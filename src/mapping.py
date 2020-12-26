#!/usr/bin/env python
# -*- coding: big5-hkscs -*-
import sys

if __name__ == "__main__":
    print("Start mapping")
    zhuyin_file_path = sys.argv[1] #"./Big5-ZhuYin.map"
    zhuyin_out_path = sys.argv[2] #"./ZhuYin-Big5.map"

    with open(zhuyin_file_path, "r", encoding="big5-hkscs") as f:
        all_char_dict = {}
        for line in f:
            line = line.strip().split(" ")
            zh_chr, zhuyin_list = line[0], line[1].split("/")
            
            if zh_chr not in all_char_dict:
                all_char_dict[zh_chr] = set()
                all_char_dict[zh_chr].add(zh_chr)
            
            for zhuyin in zhuyin_list:
                if zhuyin[0] not in all_char_dict:
                    all_char_dict[zhuyin[0]] = set()
                if zh_chr not in all_char_dict[zhuyin[0]]:
                    all_char_dict[zhuyin[0]].add(zh_chr)

    with open(zhuyin_out_path, "w", encoding="big5-hkscs") as f:
        for key, value in all_char_dict.items():
            line = ""
            for sub_value in value:
                line += f" {sub_value}"
            out = f"{key}{line}\n"
            f.write(out)