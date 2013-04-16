#include "modules/MKeyValue.h"
#include "SMJS_Plugin.h"
#include "modules/MEntities.h"
#include "CDetour/detours.h"
#include "SMJS_Entity.h"


WRAPPED_CLS_CPP(MKeyValue, SMJS_Module);

MKeyValue::MKeyValue(){
	identifier = "keyvalue";

}

void MKeyValue::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	auto obj = wrapper->ToObject();
	
}

v8::Handle<v8::Value> ParseKeyValue(const char *str);

FUNCTION_M(MKeyValue::parseKVFile)
	PSTR(filename);
	//if(strstr(*filename, "../") != NULL || strstr(*filename, "..\\") != NULL){
	if(strstr(*filename, "/") != NULL || strstr(*filename, "\\") != NULL){
		return v8::ThrowException(v8::Exception::Error(v8::String::New("Invalid file path")));
	}

	SMJS_Plugin *pl = GetPluginRunning();

	char path[512];
#if WIN32
	snprintf(path, sizeof(path), "%s\\kv\\%s", pl->GetPath(), filename);
#else
	snprintf(path, sizeof(path), "%s/kv/%s", pl->GetPath(), filename);
#endif
	
	FILE* fileHandle = fopen(path, "rb");
	if(fileHandle == NULL) THROW("Invalid file path");

	fseek(fileHandle, 0, SEEK_END);
	size_t size = ftell(fileHandle);
	rewind(fileHandle);
	
	char* source = new char[size + 1];
	for (size_t i = 0; i < size;) {
		i += fread(&source[i], 1, size - i, fileHandle);
	}
	source[size] = '\0';
	fclose(fileHandle);

	auto res = ParseKeyValue(source);
	delete source;
	return res;
END

FUNCTION_M(MKeyValue::parse)
	PSTR(str);
	return ParseKeyValue(*str);
END

v8::Handle<v8::Value> ParseKeyValue(const char *str){
	int len = strlen(str);
	int line = 1;
	int i = 0;
	int depth = 0;
	
	enum TreeType {
		Object,
		Array
	};

	std::vector<v8::Handle<v8::Value>> tree;
	std::vector<TreeType> treeType;
	std::vector<v8::Local<v8::String>> keys;

	tree.push_back(v8::Object::New());
	treeType.push_back(Object);
	keys.push_back(v8::Local<v8::String>::Local());

	keys[0].Clear();

	for(;i < len; ++i){
		char chr = str[i];
		if(chr == ' ' || chr == '\t'){
			// Ignore
		}else if(chr == '\n'){
			++line;
			if(i + 1 < len && str[i + 1] == '\r') ++i;
		}else if(chr == '\r'){
			++line;
			if(i + 1 < len && str[i + 1] == '\n') ++i;
		}else if(chr == '/' && i + 1 < len && str[i + 1] == '/'){
			i += 2;
			while(i < len && str[i] != '\n') ++i;
			--i; // Reparse the newline
		}else if(chr == '/' && i + 1 < len && str[i + 1] == '*'){
			int startLine = line;
			i += 2;
			while(i + 1 < len && (str[i] != '*' || str[i + 1] != '/')){
				if(str[i] == '\n') ++line;
				++i;
			}

			if(i + 1 == len){
				THROW_VERB("Unterminated comment at the end of the document, started at line %d", startLine);
			}
			i += 1;

		}else if(chr == '"'){
			int startIndex = i++;
			for(;i < len; ++i){
				chr = str[i];
				if(chr == '"' || chr == '\n' || chr == '\r') break;
			}

			if(i == len || chr == '\n' || chr == '\r'){
				THROW_VERB("Unterminated string at line %d", line);
			}

			auto resultString = v8::String::New(&str[startIndex + 1], i - startIndex - 1);

			if(treeType[depth] == Object){
				if(keys[depth].IsEmpty()){
					keys[depth] = resultString;
				}else{
					v8::Handle<v8::Object>::Cast(tree[depth])->Set(keys[depth], resultString);
					keys[depth].Clear();
				}
			}else{ //  if(treeType[depth] == Array)
				auto arr = v8::Handle<v8::Array>::Cast(tree[depth]);
				arr->Set(arr->Length(), resultString);
			}
		}else if(chr >= '0' && chr <= '9'){
			int startIndex = i++;
			for(; i < len; ++i){
				chr = str[i];
				if ((chr < '0' || chr > '9') && chr != '.' && chr != 'x') break;
			}

			char tmp = str[i];
			const_cast<char*>(str)[i] = '\0';
			auto number = v8::Number::New(atof(&str[i]));
			const_cast<char*>(str)[i] = tmp;

			//TODO: If the number is an invalid number, throw an error

			if(treeType[depth] == Object){
				if(keys[depth].IsEmpty()){
					THROW_VERB("A number can't be the key of a value at line %d (offset %d)", line, i);
				}else{
					v8::Handle<v8::Object>::Cast(tree[depth])->Set(keys[depth], number);
					keys[depth].Clear();
				}
			}else{ //  if(treeType[depth] == Array)
				auto arr = v8::Handle<v8::Array>::Cast(tree[depth]);
				arr->Set(arr->Length(), number);
			}

			--i; // Reparse the character that ended this number
		}else if(chr == '{'){
			if(treeType[depth] == Object){
				if(keys[depth].IsEmpty()){
					THROW_VERB("A block needs a key at line %d (offset %d)", line, i);
				}
			}

			++depth;

			auto obj = v8::Object::New();
			tree.push_back(obj);
			treeType.push_back(Object);
			keys.push_back(v8::Local<v8::String>::Local());
			keys[depth].Clear();
		}else if(chr == '}'){
			if(depth == 0){
				THROW_VERB( "Mismatching bracket at line %d", line);
			}

			if(treeType[depth] != Object){
				THROW_VERB("Mismatching brackets at line %d", line);
			}

			auto v = tree[depth];

			--depth;
			if(treeType[depth] == Object){
				v8::Handle<v8::Object>::Cast(tree[depth])->Set(keys[depth], v);
				keys[depth].Clear();
			}else{ //  if(treeType[depth] == Array)
				auto arr = v8::Handle<v8::Array>::Cast(tree[depth]);
				arr->Set(arr->Length(), v);
			}

			tree.pop_back();
			treeType.pop_back();
			keys.pop_back();
			

		}else if(chr == '['){
			if(treeType[depth] == Object){
				if(keys[depth].IsEmpty()){
					THROW_VERB("An array needs a key at line %d (offset %d)", line, i);
				}
			}

			++depth;

			auto arr = v8::Array::New();
			tree.push_back(arr);
			treeType.push_back(Array);
			keys.push_back(v8::Local<v8::String>::Local());
			keys[depth].Clear();
		}else if(chr == ']'){
			if(depth == 0){
				THROW_VERB( "Mismatching bracket at line %d", line);
			}

			if(treeType[depth] != Array){
				THROW_VERB("Mismatching brackets at line %d", line);
			}

			auto v = tree[depth];

			--depth;
			if(treeType[depth] == Object){
				v8::Handle<v8::Object>::Cast(tree[depth])->Set(keys[depth], v);
				keys[depth].Clear();
			}else if(treeType[depth] == Array){ //  if(treeType[depth] == Array)
				auto arr = v8::Handle<v8::Array>::Cast(tree[depth]);
				arr->Set(arr->Length(), v);
			}

			tree.pop_back();
			treeType.pop_back();
			keys.pop_back();
			
		}else if(chr == '/'/* && i + 1 < len && str[i + 1] == '/'*/){
			// It seems Valve's parser considers a single slash as the start of a comment
			// So let's follow that weird rule
			// i += 2;
			i += 1;
			while(i < len && str[i] != '\n' && str[i] != '\r'){
				++i;
			}
			--i;
		}else{
			THROW_VERB("Unexpected character \"%c\" at line %d (offset %d)", chr, line, i);
		}
	}

	if(depth != 0){
		THROW_VERB("Missing brackets");
	}

	return tree[0];
}