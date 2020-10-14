#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<limits.h>

enum Mode {
	MODE_T2S,
	MODE_S2T
};

typedef struct {
	int mode;
	int spacesPerTab;
	char** extensions;
	int extensionsCount;
} Options;

// test string equal
int SE(char* s1, char* s2) {
	if(strcmp(s1, s2) == 0)
		return 1;
	else
		return 0;
}

// check is string s is a positive integer (> 0)
int isPositiveInt(char* s) {
	int len = strlen(s);
	if(len <= 0)
		return 0;

	int i = 0;
	while(i < len) {
		if(s[i] < '0' || s[i] > '9')
			return 0;
		i++;
	}
	if(s[0] == '0')
		return 0;
	return 1;
}

// read a full line from file stream fp, return an allocated memory space (or NULL if reach EOF), should be free manually
char* getLine(FILE* fp) {
	char* buf = (char*)malloc(sizeof(char) * 4);
	int bufLen = 4;
	int start = 0;

	int fileSize;
	int cur = ftell(fp);
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, cur, SEEK_SET);

	// EOF
	if(fgets(buf, bufLen, fp) == NULL) {
		free(buf);
		return NULL;
	}

	while(1) {
		if(buf[strlen(buf) - 1] != '\n') {
			// reach EOF without \n as last character
			if(ftell(fp) == fileSize)
				return buf;
			// didn't reach EOF
			bufLen *= 2;
			buf = (char*)realloc(buf, sizeof(char) * bufLen);
			start = strlen(buf);
		}
		else
			return buf;
		fgets(buf + start, bufLen - start, fp);
	}
}

void copyFile(char* src, char* dest) {
	FILE* rf = fopen(src, "r");
	FILE* wf = fopen(dest, "w");
	char* line;
	while((line = getLine(rf))) {
		fprintf(wf, "%s", line);
		free(line);
	}
	fclose(rf);
	fclose(wf);
}

void processFile(char* name, Options opt) {
	FILE* swapf = fopen(".indent-converter.swap", "w");
	FILE* fp = fopen(name, "r");
	char* line;

	while((line = getLine(fp))) {
		int offset = 0;
		if(opt.mode == MODE_T2S) {
			while(line[offset] ==  '\t')
				offset++;
			int i;
			for(i = 0; i < offset; i++) {
				int j;
				for(j = 0; j < opt.spacesPerTab; j++)
					fprintf(swapf, " ");
			}
		} else {
			while(line[offset] == ' ')
				offset++;
			int i;
			for(i = 0; i < offset / opt.spacesPerTab; i++)
				fprintf(swapf, "\t");
		}

		fprintf(swapf, "%s", &line[offset]);
		free(line);
	}
	
	fclose(swapf);
	fclose(fp);

	copyFile(".indent-converter.swap", name);
	printf("MODIFY %s\n", name);
	remove(".indent-converter.swap");
}

void processDir(char* name, Options opt) {
	DIR* dir = opendir(name);
	struct dirent* entry;
	while((entry = readdir(dir))) {
		char fullPath[PATH_MAX];
		sprintf(fullPath, "%s/%s", name, entry->d_name);
		if(entry->d_type == DT_DIR && !SE(entry->d_name, ".") && !SE(entry->d_name, "..")) {
			processDir(fullPath, opt);
		} else if(entry->d_type != DT_DIR) {
			int i;
			for(i = 0; i < opt.extensionsCount; i++) {
				char* ext = opt.extensions[i];
				if(strncmp(ext, &fullPath[strlen(fullPath) - strlen(ext)], strlen(ext)) == 0)
					processFile(fullPath, opt);
			}
		}
	}
	closedir(dir);
}

void printUsage() {
	printf("Usage: indent-converter {t2s | s2t} SPACES_PER_TAB EXT ...\n");
	printf("       indent-converter --help\n\n");
	printf("       {t2s | s2t}     # Mode selection. \"Tabs to spaces\" or \"spaces to tabs\".\n");
	printf("       SPACES_PER_TAB  # Specify how many spaces should be transform to a tab. Should be a positive integer.\n");
	printf("       EXT ...         # Specify file extensions to process. Must specify at least one. (ex: .js .html)\n");
}

int main(int argc, char* argv[]) {
	if(argc == 2 && SE(argv[1], "--help"))
		printUsage();
	else {
		if(argc >= 4 && (SE(argv[1], "t2s") || SE(argv[1], "s2t")) && isPositiveInt(argv[2])) {
			Options options;
			options.mode = SE(argv[1], "t2s") ? MODE_T2S : MODE_S2T;
			options.spacesPerTab = atoi(argv[2]);
			options.extensions = &argv[3];
			options.extensionsCount = argc - 3;
			processDir(".", options);
		}
		else
			printUsage();
	}

	return 0;
}
