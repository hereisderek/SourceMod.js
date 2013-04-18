#include "extension.h"
#include "SMJS_Plugin.h"

std::vector<SMJS_Module*> modules;
std::vector<SMJS_Plugin*> plugins;
PLUGIN_ID numPlugins = 0;

inline const char* ToCString(const String::Utf8Value& value) {
  return *value ? *value : "NULL";
}

void SMJS_AddModule(SMJS_Module *module){
	modules.push_back(module);
}


SMJS_Plugin *GetPlugin(PLUGIN_ID id){
	if(id < 0 || id > (int) plugins.size()) return NULL;
	return plugins[id];
}

int GetNumPlugins(){
	return plugins.size();
}

SMJS_Plugin* SMJS_Plugin::GetPluginByDir(const char *dir){
	for(auto it = plugins.begin(); it != plugins.end(); ++it){
		if(*it == NULL) continue;
		if(strcmp((*it)->dir.c_str(), dir) == 0) return *it;
	}

	return NULL;
}

SMJS_Plugin::SMJS_Plugin(const char *name){
	apiVersion = SMJS_API_VERSION;
	isSandboxed = true;
	id = numPlugins++;
	plugins.push_back(this);

	isolate = mainIsolate; //v8::Isolate::New();

	HandleScope handle_scope(isolate);

	Handle<ObjectTemplate> global = v8::ObjectTemplate::New();

	global->Set(v8::String::New("print"), v8::FunctionTemplate::New(JSN_Print));
	global->Set(v8::String::New("require"), v8::FunctionTemplate::New(JSN_Require));

	context = v8::Context::New(NULL, global);
	context->SetEmbedderData(1, v8::External::New(this));
}

void SMJS_Plugin::LoadModules(){
	HandleScope handle_scope(isolate);
	Context::Scope context_scope(context);

	for(auto it = modules.begin(); it != modules.end(); ++it) {
		SMJS_Module *module = (*it);
		if(!module->sandboxed && isSandboxed) continue;
		context->Global()->Set(v8::String::New(module->identifier.c_str()), module->GetWrapper(this), ReadOnly);
	}
	
}

SMJS_Plugin::~SMJS_Plugin(){
	for(auto it = destroyCallbackFuncs.begin(); it != destroyCallbackFuncs.end(); ++it){
		(*it)(this);
	}

	for(auto it = destroyCallbackHandlers.begin(); it != destroyCallbackHandlers.end(); ++it){
		(*it)->OnPluginDestroyed(this);
	}

	//context.MakeWeak(isolate, NULL, NULL);
	context.Dispose();
	//v8::V8::TerminateExecution(isolate);
	//isolate->Dispose();
	plugins[id] = NULL;
}

void SMJS_Plugin::RegisterDestroyCallback(DestroyCallback func){
	destroyCallbackFuncs.push_back(func);
}

void SMJS_Plugin::RegisterDestroyCallback(IPluginDestroyedHandler *ptr){
	destroyCallbackHandlers.push_back(ptr);
}

std::vector<v8::Persistent<v8::Function>>* SMJS_Plugin::GetHooks(char const *type){
	std::string typeStd(type);
	auto it = hooks.find(typeStd);
	if(it != hooks.end()){
		return &it->second;
	}

	
	hooks.insert(std::make_pair(typeStd, std::vector<v8::Persistent<v8::Function>>()));

	auto vec = GetHooks(type);

	HandleScope handle_scope(isolate);
	Context::Scope context_scope(context);
	auto g = context->Global()->Get(v8::String::New(type));
	if(!g.IsEmpty() && g->IsFunction()){
		vec->push_back(v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(g)));
	}

	return vec;
}

std::vector<v8::Persistent<v8::Function>>* SMJS_Plugin::GetEventHooks(char const *type){
	std::string typeStd(type);
	auto it = eventHooks.find(typeStd);
	if(it != eventHooks.end()){
		return &it->second;
	}

	
	eventHooks.insert(std::make_pair(typeStd, std::vector<v8::Persistent<v8::Function>>()));
	return GetEventHooks(type);
}

std::vector<v8::Persistent<v8::Function>>* SMJS_Plugin::GetEventPostHooks(char const *type){
	std::string typeStd(type);
	auto it = eventPostHooks.find(typeStd);
	if(it != eventPostHooks.end()){
		return &it->second;
	}

	
	eventPostHooks.insert(std::make_pair(typeStd, std::vector<v8::Persistent<v8::Function>>()));
	return GetEventHooks(type);
}

bool SMJS_Plugin::RunString(const char* name, const char *source, bool asGlobal){
	HandleScope handle_scope(isolate);
	Context::Scope context_scope(context);

	TryCatch try_catch;
	v8::ScriptOrigin origin(String::New(name), v8::Integer::New(0), v8::Integer::New(0));
	Handle<Script> script;
	if(asGlobal){
		script = Script::Compile(v8::String::New(source), &origin);
	}else{
		char *buffer = new char[strlen(source) + 100];
		strcpy(buffer, "(function(global){");
		strcat(buffer, source);
		strcat(buffer, "})(this);");
		script = Script::Compile(v8::String::New(buffer), &origin);
		delete buffer;
	}

	if(script.IsEmpty()) {
		// Print errors that happened during compilation.
		ReportException(&try_catch);
		return false;
	} else {
		Handle<Value> result = script->Run();
		if (result.IsEmpty()) {
			ReportException(&try_catch);
			return false;
		}

		return true;
	}
}

void SMJS_Plugin::ReportException(TryCatch* try_catch){
	HandleScope handle_scope(isolate);

	fprintf(stderr, "----------------------------- JAVASCRIPT EXCEPTION -----------------------------");

	String::Utf8Value exception(try_catch->Exception());
	const char* exception_string = ToCString(exception);
	Handle<Message> message = try_catch->Message();
	if (message.IsEmpty()) {
		// V8 didn't provide any extra information about this error; just
		// print the exception.
		fprintf(stderr, "%s\n", exception_string);
	} else {
		// Print (filename):(line number): (message).
		String::Utf8Value filename(message->GetScriptResourceName());
		const char* filename_string = ToCString(filename);
		int linenum = message->GetLineNumber();
		fprintf(stderr, "%s:%d: %s\n", filename_string, linenum, exception_string);
		// Print line of source code.
		String::Utf8Value sourceline(message->GetSourceLine());
		const char* sourceline_string = ToCString(sourceline);
		fprintf(stderr, "%s\n", sourceline_string);
		// Print wavy underline (GetUnderline is deprecated).
		int start = message->GetStartColumn();
		for (int i = 0; i < start; i++) {
			fprintf(stderr, " ");
		}
		int end = message->GetEndColumn();
		for (int i = start; i < end; i++) {
			fprintf(stderr, "^");
		}
		fprintf(stderr, "\n");
		String::Utf8Value stack_trace(try_catch->StackTrace());
		if (stack_trace.length() > 0) {
			const char* stack_trace_string = ToCString(stack_trace);
			fprintf(stderr, "%s\n", stack_trace_string);
		}
	}

	fprintf(stderr, "--------------------------------------------------------------------------------");
}

bool SMJS_Plugin::LoadFile(const char* file, bool asGlobal){
	HandleScope handle_scope(isolate);
	
	char path[512];
#if WIN32
	snprintf(path, sizeof(path), "%s\\%s", GetPath(), file);
#else
	snprintf(path, sizeof(path), "%s/%s", GetPath(), file);
#endif
	
	FILE* fileHandle = fopen(path, "rb");
	if(fileHandle == NULL) return false;

	fseek(fileHandle, 0, SEEK_END);
	size_t size = ftell(fileHandle);
	rewind(fileHandle);
	
	char* source = new char[size + 1];
	for (size_t i = 0; i < size;) {
		i += fread(&source[i], 1, size - i, fileHandle);
	}
	source[size] = '\0';
	fclose(fileHandle);

	bool res = RunString(path, source, asGlobal);
	delete[] source;
	return res;
}

void SMJS_Plugin::CheckApi(){
	char *file = "API_VERSION";
	char path[PLATFORM_MAX_PATH];
#if WIN32
	snprintf(path, sizeof(path), "%s\\%s", GetPath(), file);
#else
	snprintf(path, sizeof(path), "%s/%s", GetPath(), file);
#endif
	FILE* fileHandle = fopen(path, "r");
	if(fileHandle == NULL){
		fileHandle = fopen(path, "w+");
		if(fileHandle == NULL) return;

		char str[8];
		snprintf(str, sizeof(str), "%d", apiVersion);
		fwrite(str, sizeof(str[0]), strlen(str), fileHandle);
		fclose(fileHandle);
		return;
	}

	char str[8];
	fread(str, sizeof(str[0]), sizeof(str) / sizeof(str[0]), fileHandle);
	fclose(fileHandle);
	apiVersion = atoi(str);
}

Handle<Value> JSN_Print(const Arguments& args) {
	HandleScope handle_scope(args.GetIsolate());

	bool first = true;
	for (int i = 0; i < args.Length(); i++) {
		if (first) {
			first = false;
		} else {
			printf(" ");
		}
		String::Utf8Value str(args[i]);
		const char* cstr = ToCString(str);
		printf("%s", cstr);
	}
	printf("\n");
	fflush(stdout);
	return Undefined();
}

FUNCTION(JSN_Require)
	ARG_COUNT(1);
	ARG_str(filename, 0);

	if(strstr(*filename, "../") != NULL || strstr(*filename, "..\\") != NULL){
		return v8::ThrowException(v8::Exception::Error(v8::String::New("Invalid file path")));
	}

	SMJS_Plugin *plugin = GetPluginRunning();
	if(!plugin->LoadFile(*filename, false)){
		return v8::ThrowException(v8::Exception::Error(v8::String::New("Failed to load file")));
	}

	return v8::Undefined();
END