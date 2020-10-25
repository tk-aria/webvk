#include <napi.h>
#include <vector>
#include "zstd-wrapper.h"

// std::vector → Buffer に変換する
Napi::Buffer<uint8_t> to_buffer(napi_env env, std::vector<uint8_t>& data)
{
    // NOTE: Newだと外部メモリのビューになる。メモリ管理をNode.jsに任せるため、Copyで新規メモリを割り当てる
    return Napi::Buffer<uint8_t>::Copy(env, data.data(), data.size());
}

// Value → std::vector に変換する (ArrayBuffer/Buffer/TypedArrayのみ対応)
std::vector<uint8_t> to_vector(Napi::Value value)
{
    auto env = value.Env();

    if (value.IsArrayBuffer()) {
        // ArrayBuffer
        auto buffer = value.As<Napi::ArrayBuffer>();
        const auto data = static_cast<const uint8_t*>(buffer.Data());
        return std::vector<uint8_t>(data, data + buffer.ByteLength());
    }
    else if (value.IsTypedArray()) {    // NOTE: BufferもTypedArrayとしてまとめて処理する
        // TypedArray or Buffer
        auto array = value.As<Napi::TypedArray>();
        auto buffer = array.ArrayBuffer();
        const auto data = static_cast<const uint8_t*>(buffer.Data());
        return std::vector<uint8_t>(data, data + array.ByteLength());
    }

    // その他のデータ型は未対応
    Napi::TypeError::New(env, "Unsupported data type").ThrowAsJavaScriptException();
    return std::vector<uint8_t>();
}

// ラッパークラス
class JsZstd : public Napi::ObjectWrap<JsZstd> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    JsZstd(const Napi::CallbackInfo& info);

private:
    static Napi::FunctionReference s_constructor;

    Napi::Value compress(const Napi::CallbackInfo& info);
    Napi::Value decompress(const Napi::CallbackInfo& info);

    Zstd zstd_;
};

// 注意: JavaScriptのコンストラクタオブジェクト
// クラス宣言とは別に定義する必要があるので注意 (しないとビルドエラー or 実行時エラーになる)
Napi::FunctionReference JsZstd::s_constructor;

// 初期化処理、JavaScriptからクラスを利用できるようにする
Napi::Object JsZstd::Init(Napi::Env env, Napi::Object exports) {
    // JavaScriptのクラスを定義する
    Napi::Function func = DefineClass(env, "Zstd", {
        InstanceMethod("compress", &JsZstd::compress),
        InstanceMethod("decompress", &JsZstd::decompress),
    });

    // コンストラクタを用意する
    s_constructor = Napi::Persistent(func);
    // コンストラクタは破棄されないようにする
    s_constructor.SuppressDestruct();

    // Zstd という名前でクラスをエクスポートする
    exports.Set("Zstd", func);
    return exports;
}

// ラッパーオブジェクトのコンストラクタ
JsZstd::JsZstd(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<JsZstd>(info), zstd_() {
    // nop
}

// 圧縮関数: 対象のデータを受け取り、圧縮結果を返す
Napi::Value JsZstd::compress(const Napi::CallbackInfo &info) {
    auto env = info.Env();


    //// 引数チェック: 対象のデータが指定されていない場合は続行不可
    //if (info.Length() < 1) {
    //    Napi::TypeError::New(env, "Wrong number of arguments, expected 1")
    //        .ThrowAsJavaScriptException();
    //    return env.Null();
    //}
//
    //// 第1引数: 対象のデータを取得
    //auto original = to_vector(info[0]);
    //if (env.IsExceptionPending()) {
    //    return env.Null();
    //}
//
    //// 第2引数: 圧縮レベルが指定されている場合は取得する。省略時はデフォルトの圧縮レベルにする
    //int level = Zstd::defaultCompressionLevel();
    //if (info.Length() >= 2) {
    //    if (!info[1].IsNumber()) {
    //        Napi::TypeError::New(env, "Wrong second argument, must be a number")
    //            .ThrowAsJavaScriptException();
    //        return env.Null();
    //    }
//
    //    level = info[1].As<Napi::Number>().Int32Value();
    //}
//
    //// 圧縮する
    //std::vector<uint8_t> compressed;
    //if (!zstd_.compress(compressed, original, level)) {
    //    Napi::TypeError::New(env, "Compression error")
    //        .ThrowAsJavaScriptException();
    //    return env.Null();
    //}
//
    //// 圧縮した結果をBufferに変換して返す
    //return to_buffer(env, compressed).As<Napi::Value>();

	return env.Null();
}

// 伸長関数: 対象のデータを受け取り、伸長結果を返す
Napi::Value JsZstd::decompress(const Napi::CallbackInfo &info) {
    auto env = info.Env();

    //// 引数チェック: 対象のデータが指定されていない場合は続行不可
    //if (info.Length() < 1) {
    //    Napi::TypeError::New(env, "Wrong number of arguments, expected 1")
    //        .ThrowAsJavaScriptException();
    //    return env.Null();
    //}

    //// 第1引数: 対象のデータを取得
    //auto compressed = to_vector(info[0]);
    //if (env.IsExceptionPending()) {
    //    return env.Null();
    //}

    //// 伸長する
    //std::vector<uint8_t> original;
    //if (!zstd_.decompress(original, compressed)) {
    //    Napi::TypeError::New(env, "Decompression error")
    //        .ThrowAsJavaScriptException();
    //    return env.Null();
    //}

    //// 伸長した結果をBufferに変換して返す
    //return to_buffer(env, original).As<Napi::Value>();

	return env.Null();
}

// モジュールの初期化処理
Napi::Object init(Napi::Env env, Napi::Object exports)
{
    // ラッパークラスを初期化する
    JsZstd::Init(env, exports);

    // エクスポート結果を返却
    return exports;
}

// モジュールを登録する
NODE_API_MODULE(napi_zstd, init);
