static TShader<_CLS>::IDataBindingFactory* Initialize(boo::GLDataFactory::Context& ctx);
#if _WIN32
static TShader<_CLS>::IDataBindingFactory* Initialize(boo::ID3DDataFactory::Context& ctx);
#endif
#if BOO_HAS_METAL
static TShader<_CLS>::IDataBindingFactory* Initialize(boo::MetalDataFactory::Context& ctx);
#endif
#if BOO_HAS_VULKAN
static TShader<_CLS>::IDataBindingFactory* Initialize(boo::VulkanDataFactory::Context& ctx);
#endif
static void Shutdown();
