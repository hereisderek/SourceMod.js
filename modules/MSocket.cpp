#include "modules/MSocket.h"
#include "SMJS_Plugin.h"
#include "SMJS_SimpleWrapped.h"
#include <algorithm>

class JSSocket;

WRAPPED_CLS_CPP(MSocket, SMJS_Module);

MSocket *g_MSocket;

v8::Persistent<v8::Object> CreateBuffer(v8::Isolate *isolate, uint8_t *ptr, size_t len);
void FreeBuffer(v8::Isolate *isolate, v8::Persistent<v8::Object> buffer);
void FreeBufferCallback(Isolate* isolate, v8::Persistent<v8::Value> object, void *parameter);

MSocket::MSocket(){
	identifier = "socket";
	sandboxed = false;
	g_MSocket = this;

	smutils->AddGameFrameHook(OnGameFrame);
}

void MSocket::OnWrapperAttached(SMJS_Plugin *plugin, v8::Persistent<v8::Value> wrapper){
	
}

void MSocket::OnPluginDestroyed(SMJS_Plugin *plugin){

}

// JSSocket

class JSSocket : public SMJS_SimpleWrapped {
public:
	CActiveSocket socket;
	bool connected;
	bool used;

	JSSocket(SMJS_Plugin *pl) : SMJS_SimpleWrapped(pl) {
		connected = false;
		used = false;

		socket.Initialize();
		socket.SetNonblocking();

		g_MSocket->sockets.push_back(this);
	}

	~JSSocket(){
		g_MSocket->sockets.erase(std::remove(g_MSocket->sockets.begin(), g_MSocket->sockets.end(), this), g_MSocket->sockets.end());
	}


	FUNCTION_DECL(connectW);
	FUNCTION_DECL(close);
	FUNCTION_DECL(writeString);
	FUNCTION_DECL(writeBuffer);

	SIMPLE_WRAPPED_CLS(JSSocket, SMJS_SimpleWrapped) {
		temp->SetClassName(v8::String::New("Socket"));

		proto->Set("onClose", v8::Null());
		proto->Set("onData", v8::Null());
		proto->Set(v8::String::New("connect"), v8::FunctionTemplate::New(connectW));
		WRAPPED_FUNC(close);
		WRAPPED_FUNC(writeString);
		WRAPPED_FUNC(writeBuffer);
	}
};

void MSocket::OnThink(bool finalTick){
	OnGameFrame(false);
}

SIMPLE_WRAPPED_CLS_CPP(JSSocket, SMJS_SimpleWrapped);

FUNCTION_M(JSSocket::connectW)
	GET_INTERNAL(JSSocket*, self);
	ARG_COUNT(2);
	PSTR(hostname);
	PINT(port);

	if(self->connected) THROW("Socket already connected");
	if(self->used) THROW("Socket has already been used");

	self->used = true;
	auto res = self->socket.Open((const uint8*) *hostname, port);
	self->connected = res;
	return v8::Boolean::New(res);
END

FUNCTION_M(JSSocket::writeString)
	GET_INTERNAL(JSSocket*, self);
	ARG_COUNT(1);
	PSTR(str);
	
	return v8::Int32::New(self->socket.Send((uint8*) *str, str.length()));
END

FUNCTION_M(JSSocket::writeBuffer)
	GET_INTERNAL(JSSocket*, self);
	ARG_COUNT(2);
	POBJ(obj);
	PINT(len);

	auto h = obj->GetHiddenValue(v8::String::New("SMJS::bufferPtr"));
	if(h.IsEmpty()) THROW("Object is not a buffer");
	uint8_t *ptr = (uint8_t*) v8::Handle<v8::External>::Cast(h)->Value();
	
	return v8::Int32::New(self->socket.Send(ptr, min(len, obj->GetHiddenValue(v8::String::New("SMJS::bufferPtrLen"))->Int32Value())));
END

FUNCTION_M(JSSocket::close)
	GET_INTERNAL(JSSocket*, self);
	if(!self->connected) THROW("Socket is not connected");

	auto res = self->socket.Close();
	self->connected = !res;
	return v8::Boolean::New(res);
END

/////////////

FUNCTION_M(MSocket::create)
	return (new JSSocket(GetPluginRunning()))->GetWrapper();
END

FUNCTION_M(MSocket::createBuffer)
PINT(len);
	auto buffer = CreateBuffer(GetPluginRunning()->GetIsolate(), new uint8_t[len], len);
	FreeBuffer(GetPluginRunning()->GetIsolate(), buffer);
	return buffer;
END

void MSocket::OnGameFrame(bool simulating){
	for(auto it = g_MSocket->sockets.begin(); it != g_MSocket->sockets.end(); ++it){
		auto jssocket = *it;
		auto &socket = jssocket->socket;
		
		auto pl = jssocket->GetPlugin();
		auto context = pl->GetContext();
		HandleScope handle_scope(pl->GetIsolate());
		Context::Scope context_scope(context);

		if(socket.IsSocketValid()){
			while(true){
				int received = socket.Receive(4096);
				if(received <= 0) break;
				
				
				auto callback = jssocket->GetWrapper()->ToObject()->Get(v8::String::New("onData"));

				if(callback->IsFunction()){
					uint8_t *ptr = new uint8_t[received];
					auto buffer = CreateBuffer(pl->GetIsolate(), ptr, received);
					memcpy(ptr, socket.GetData(), received);
					ptr[received] = '\0';

					auto func = v8::Handle<v8::Function>::Cast(callback);
					
					v8::Handle<v8::Value> arg = buffer;
					func->Call(context->Global(), 1, &arg);

					FreeBuffer(pl->GetIsolate(), buffer);
				}
			}
		}else if(jssocket->connected){
			jssocket->connected = false;

			auto callback = jssocket->GetWrapper()->ToObject()->Get(v8::String::New("onClose"));

			if(callback->IsFunction()){
				auto func = v8::Handle<v8::Function>::Cast(callback);
				func->Call(context->Global(), 0, NULL);
			}
		}
	}
}

FUNCTION_M(BufferWriteString)
	PINT(offset);
	PUTF8(str);

	auto buffer = JS_THIS->ToObject();

	uint8_t *ptr = (uint8_t*) v8::Handle<v8::External>::Cast(buffer->GetHiddenValue(v8::String::New("SMJS::bufferPtr")))->Value();
	int len = buffer->GetHiddenValue(v8::String::New("SMJS::bufferPtrLen"))->Int32Value();
	if(offset + str.length() >= len){
		int newLen = offset + str.length();
		uint8_t *newPtr = new uint8_t[newLen];
		buffer->ForceSet(v8::String::New("length"), v8::Int32::New(newLen), ReadOnly);
		buffer->SetIndexedPropertiesToExternalArrayData(ptr, v8::kExternalUnsignedByteArray, newLen);
		buffer->SetHiddenValue(v8::String::New("SMJS::bufferPtr"), v8::External::New(ptr));
		buffer->SetHiddenValue(v8::String::New("SMJS::bufferPtrLen"), v8::Int32::New(newLen));

		memcpy(newPtr, ptr, len);
		delete[] ptr;

		ptr = newPtr;
		len = newLen;
	}

	memcpy(ptr + offset, *str, str.length());
	return v8::Int32::New(str.length());
END

FUNCTION_M(BufferReadString)
	PINT(offset);
	PINT(len);

	auto buffer = JS_THIS->ToObject();
	uint8_t *ptr = (uint8_t*) v8::Handle<v8::External>::Cast(buffer->GetHiddenValue(v8::String::New("SMJS::bufferPtr")))->Value();
	int bufLen = buffer->GetHiddenValue(v8::String::New("SMJS::bufferPtrLen"))->Int32Value();
	if(offset + len >= bufLen) len -= bufLen - offset - len;

	return v8::String::New((char*)(ptr + offset), len);
END

v8::Persistent<v8::Object> CreateBuffer(v8::Isolate *isolate, uint8_t *ptr, size_t len){
	auto buffer = v8::Persistent<v8::Object>::New(v8::Object::New());
	
	buffer->Set(v8::String::New("readString"), v8::FunctionTemplate::New(BufferReadString)->GetFunction());
	buffer->Set(v8::String::New("writeString"), v8::FunctionTemplate::New(BufferWriteString)->GetFunction());
	buffer->ForceSet(v8::String::New("length"), v8::Int32::New(len), ReadOnly);
	buffer->SetIndexedPropertiesToExternalArrayData(ptr, v8::kExternalUnsignedByteArray, len);
	buffer->SetHiddenValue(v8::String::New("SMJS::bufferPtr"), v8::External::New(ptr));
	buffer->SetHiddenValue(v8::String::New("SMJS::bufferPtrLen"), v8::Int32::New(len));

	isolate->AdjustAmountOfExternalAllocatedMemory(len);

	return buffer;
}

void FreeBuffer(v8::Isolate *isolate, v8::Persistent<v8::Object> buffer){
	buffer.MakeWeak(isolate, NULL, FreeBufferCallback);
}

void FreeBufferCallback(Isolate* isolate, v8::Persistent<v8::Value> object, void *parameter){
	delete v8::Handle<v8::External>::Cast(object->ToObject()->GetHiddenValue(v8::String::New("SMJS::bufferPtr")))->Value();
}