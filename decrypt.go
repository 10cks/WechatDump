package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/hmac"
	"crypto/sha1"
	"encoding/hex"
	"flag"
	"io/ioutil"
	"log"
	"os"

	"golang.org/x/crypto/pbkdf2"
)

const (
	SQLITE_FILE_HEADER = "SQLite format 3\x00"
	IV_SIZE            = 16
	HMAC_SHA1_SIZE     = 20
	KEY_SIZE           = 32
	DEFAULT_PAGESIZE   = 4096
	DEFAULT_ITER       = 64000
)

func xor(a, b []byte) (out []byte) {
	out = make([]byte, len(a))
	for i := 0; i < len(a); i++ {
		out[i] = a[i] ^ b[i]
	}
	return out
}

func main() {
	key := flag.String("k", "", "key")
	file := flag.String("d", "", "file")
	outputFile := flag.String("o", "", "output file") // 新增的 -o 选项
	flag.Parse()

	password, _ := hex.DecodeString(*key)

	data, err := ioutil.ReadFile(*file)
	if err != nil {
		log.Fatalf("ReadFile: %v", err)
	}

	salt := data[:16]
	key1 := pbkdf2.Key([]byte(password), salt, DEFAULT_ITER, KEY_SIZE, sha1.New)
	first := data[16:DEFAULT_PAGESIZE]

	macSalt := xor(salt, []byte{0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a, 0x3a})
	macKey := pbkdf2.Key(key1, macSalt, 2, KEY_SIZE, sha1.New)

	hashMac := hmac.New(sha1.New, macKey)
	hashMac.Write(first[:len(first)-32])
	hashMac.Write([]byte{0x01, 0x00, 0x00, 0x00})

	if hmac.Equal(hashMac.Sum(nil), first[len(first)-32:len(first)-12]) {
		log.Println("Decryption Success")
	} else {
		log.Println("Password Error")
	}

	dataList := make([][]byte, 0)
	for i := DEFAULT_PAGESIZE; i < len(data); i += DEFAULT_PAGESIZE {
		dataList = append(dataList, data[i:i+DEFAULT_PAGESIZE])
	}

	f, err := os.Create(*outputFile) // 使用 -o 选项指定的文件名
	if err != nil {
		log.Fatalf("Create file: %v", err)
	}

	f.Write([]byte(SQLITE_FILE_HEADER))

	block, _ := aes.NewCipher(key1)
	t := cipher.NewCBCDecrypter(block, first[len(first)-48:len(first)-32])
	firstDecrypted := make([]byte, len(first)-48)
	t.CryptBlocks(firstDecrypted, first[:len(first)-48])

	f.Write(firstDecrypted)
	f.Write(first[len(first)-48:])

	for _, data := range dataList {
		t = cipher.NewCBCDecrypter(block, data[len(data)-48:len(data)-32])
		dataDecrypted := make([]byte, len(data)-48)
		t.CryptBlocks(dataDecrypted, data[:len(data)-48])

		f.Write(dataDecrypted)
		f.Write(data[len(data)-48:])
	}

	f.Close()
}
