#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <typeinfo>

//#define OBFS_XOR_EXTERN

#ifndef OBFS_XOR_EXTERN
#define OBFS_XOR_MANUAL
#endif

using namespace std;
using namespace llvm;

namespace
{
  class GlobalString
  {
  public:
    llvm::GlobalVariable* Glob;
    uint32_t index;
    int32_t type;
    int32_t string_length;
    static const int32_t SIMPLE_STRING_TYPE = 1;
    static const int32_t STRUCT_STRING_TYPE = 2;

    GlobalString(GlobalVariable* Glob, int32_t length) : Glob(Glob), index(-1), string_length(length), type(SIMPLE_STRING_TYPE) {}
    GlobalString(GlobalVariable* Glob, uint32_t index, int32_t length) : Glob(Glob), index(index), string_length(length), type(STRUCT_STRING_TYPE) {}
  };

  Function*
  createDecodeStubFunc(Module &M, vector<GlobalString*> &GlobalStrings, Function *DecodeFunc)
  {
    auto &Ctx = M.getContext();
    // Add DecodeStub function
    FunctionCallee DecodeStubCallee = M.getOrInsertFunction("decode_stub",
							    Type::getVoidTy(Ctx)); // RET
    
    Function *DecodeStubFunc = cast<Function>(DecodeStubCallee.getCallee());
    DecodeStubFunc->setCallingConv(CallingConv::C);

    // Create entry block
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", DecodeStubFunc);
    IRBuilder<> Builder(BB);

    // Add calls to decode every encoded global
    for(GlobalString *GlobString : GlobalStrings)
    {
      if(GlobString->type == GlobString->SIMPLE_STRING_TYPE)
      {
	auto *StrPtr = Builder.CreatePointerCast(GlobString->Glob, PointerType::get(Ctx, 8));
	llvm::Value *le = llvm::ConstantInt::get( llvm::IntegerType::getInt32Ty(Ctx),
						  GlobString->string_length);			
	Builder.CreateCall(DecodeFunc, {StrPtr, le});
      }
      else if(GlobString->type == GlobString->STRUCT_STRING_TYPE)
      {
	llvm::StructType* strStructTy = llvm::StructType::create(Ctx, "struct_string");
	auto *String = Builder.CreateStructGEP(strStructTy, GlobString->Glob, GlobString->index);
	auto *StrPtr = Builder.CreatePointerCast(String, PointerType::get(Ctx, 8));
	llvm::Value *le = llvm::ConstantInt::get( llvm::IntegerType::getInt32Ty(Ctx),
						  GlobString->string_length);
	Builder.CreateCall(DecodeFunc, {StrPtr, le});
      }
    }
    Builder.CreateRetVoid();

    return DecodeStubFunc;
  }
  
  // void decode(char *string, int length) {
  //     char *p = string;
  //     while (p && length-- > 0) {
  //         *(p++) ^= 0xAF; 
  //     }
  // }
  //

  // $$$ clang-18 -O1 -emit-llvm decodeStub.c -c -o decodeStube.bc && llvm-dis-18 decodeStub.bc
  
  //   define dso_local void @decode(ptr noundef %0, i32 noundef %1) local_unnamed_addr #0 {
  //   %3 = icmp ne ptr %0, null
  //   %4 = icmp sgt i32 %1, 0
  //   %5 = and i1 %3, %4
  //   br i1 %5, label %6, label %14

  // 6:                                                ; preds = %2, %6
  //   %7 = phi ptr [ %10, %6 ], [ %0, %2 ]
  //   %8 = phi i32 [ %9, %6 ], [ %1, %2 ]
  //   %9 = add nsw i32 %8, -1
  //   %10 = getelementptr inbounds i8, ptr %7, i64 1
  //   %11 = load i8, ptr %7, align 1
  //   %12 = xor i8 %11, -81
  //   store i8 %12, ptr %7, align 1
  //   %13 = icmp ugt i32 %8, 1
  //   br i1 %13, label %6, label %14

  // 14:                                               ; preds = %6, %2
  //   ret void
  // }
  
#ifdef OBFS_XOR_MANUAL

  Function*
  createDecodeFunc(Module &M)
  {
    outs() << "[INFO] Building decode function inline." << "\n";
    
    auto &Ctx = M.getContext();
    FunctionCallee DecodeFuncCallee = M.getOrInsertFunction("decode", 
							    Type::getVoidTy(Ctx), // RET
							    PointerType::get(Ctx, 8), // ARG
							    Type::getInt32Ty(Ctx)); // ARG

    Function *DecodeFunc = cast<Function>(DecodeFuncCallee.getCallee());
    DecodeFunc->setCallingConv(CallingConv::C);

    // Name DecodeFunc arguments
    Function::arg_iterator Args = DecodeFunc->arg_begin();
    Value *var00 = Args++;
    Value *var01 = Args++;
	
    BasicBlock *bb02 = BasicBlock::Create(Ctx, "bb02", DecodeFunc);
    
    IRBuilder<> *Builder02 = new IRBuilder<>(bb02);
    auto *var03 = Builder02->CreateICmpNE( var00,
					   Constant::getNullValue(PointerType::get(Ctx, 8)),
					   "var03");
    auto *var04 = Builder02->CreateICmpSGT( var01,
					    ConstantInt::get(IntegerType::get(Ctx, 32), 0),
					    "var04");
    auto *var05 = Builder02->CreateAnd(var04, var03, "var05");
    
    BasicBlock *bb06 = BasicBlock::Create(Ctx, "bb06", DecodeFunc);
    BasicBlock *bb14 = BasicBlock::Create(Ctx, "bb14", DecodeFunc);
    Builder02->CreateCondBr(var05, bb06, bb14);

    IRBuilder<> *Builder06 = new IRBuilder<>(bb06);
    PHINode *phi_var07 = Builder06->CreatePHI(PointerType::get(Ctx, 8), 2, "var07");
    PHINode *phi_var08 = Builder06->CreatePHI(Type::getInt32Ty(Ctx), 2, "var08");
    auto *var09 = Builder06->CreateNSWAdd( phi_var08,
					   ConstantInt::getSigned(IntegerType::get(Ctx, 32), -1),
					   "var09");
    auto *var10 = Builder06->CreateGEP( IntegerType::get(Ctx, 8),
					phi_var07,
					ConstantInt::get(IntegerType::get(Ctx, 32), 1),
					"var10");

    auto *var11 = Builder06->CreateLoad( IntegerType::get(Ctx, 8), phi_var07, "var02");
		
    auto *var12 = Builder06->CreateXor( var11,
					ConstantInt::getSigned(IntegerType::get(Ctx, 8), 0xAF),
					"var12");
		
    Builder06->CreateStore(var12, phi_var07);
    auto *var13 = Builder06->CreateICmpSGT( phi_var08,
					    ConstantInt::get(IntegerType::get(Ctx, 32), 1),
					    "var13");

    Builder06->CreateCondBr(var13, bb06, bb14);
	
    IRBuilder<> *Builder14 = new IRBuilder<>(bb14);
    Builder14->CreateRetVoid();
    // %7 = phi i8* [ %10, %6 ], [ %0, %2 ]
    phi_var07->addIncoming(var00, bb02);
    phi_var07->addIncoming(var10, bb06);
    // %8 = phi i32 [ %9, %6 ], [ %1, %2 ]
    phi_var08->addIncoming(var01, bb02);
    phi_var08->addIncoming(var09, bb06);

    return DecodeFunc;
  }
  
#endif // END OBFS_MANUAL

  
#ifdef OBFS_XOR_EXTERN
  
  llvm::Function*
  createDecodeFunc(llvm::Module &M)
  {
    outs() << "[INFO] Loading decode function from external source." << "\n";
    
    llvm::LLVMContext* Ctx = &M.getContext();
    llvm::SMDiagnostic Err;

    // parse the bytecode into a temp module, and get the function ref to clone
    std::unique_ptr<llvm::Module> NuMod = llvm::parseIRFile("./decodeStub.bc", Err, *Ctx);
    llvm::Function* NuDecodeFunc = NuMod->getFunction("decode");

    // create the hollow function in the main module
    llvm::Function* DecodeFunc = llvm::Function::Create( NuDecodeFunc->getFunctionType(),
							   NuDecodeFunc->getLinkage(),
							   NuDecodeFunc->getName(),
							   M );

    llvm::ValueToValueMapTy VMap;

    Function::arg_iterator DestI = DecodeFunc->arg_begin();
    for( const Argument & I : NuDecodeFunc->args())
      if( VMap.count(&I) == 0) // get arguments?
      {
	DestI->setName(I.getName()); // copy the name over
	VMap[&I] = &*DestI++;        // add mapping to VMap
      }
    
    llvm::SmallVector<llvm::ReturnInst *, 8> Returns;

    // now clone it into the main module
    llvm::CloneFunctionInto( DecodeFunc, NuDecodeFunc, VMap,
			     llvm::CloneFunctionChangeType::GlobalChanges,
			     Returns );

    return M.getFunction("decode"); // return the reference in the module
  }
  
#endif // OBFS_XOR_EXTERN

  char* 
  EncodeString(const char* Data, unsigned int Length)
  {
    // Encode string
    char *NewData = (char*)malloc(Length);
    for(unsigned int i = 0; i < Length; i++)
      NewData[i] = Data[i] ^ 0xAF;
    
    return NewData;
  }
  
  void
  createDecodeStubBlock(Function *F, Function *DecodeStubFunc)
  {
    auto &Ctx = F->getContext();
    BasicBlock &EntryBlock = F->getEntryBlock();

    // Create new block
    BasicBlock *NewBB = BasicBlock::Create(Ctx, "DecodeStub", EntryBlock.getParent(), &EntryBlock);
    IRBuilder<> Builder(NewBB);

    // Call stub func instruction
    Builder.CreateCall(DecodeStubFunc);
    // Jump to original entry block
    Builder.CreateBr(&EntryBlock);
  }


  vector<GlobalString*>
  encodeGlobalStrings(Module& M)
  {
    vector<GlobalString*> GlobalStrings;
    auto &Ctx = M.getContext();

    // Encode all global strings
    for(GlobalVariable &Glob : M.globals())
    {
      // Ignore external globals & uninitialized globals.
      if (!Glob.hasInitializer() || Glob.hasExternalLinkage())
	continue;

      // Unwrap the global variable to receive its value
      Constant *Initializer = Glob.getInitializer();

      // Check if its a string
      if (isa<ConstantDataArray>(Initializer))
      {
	auto CDA = cast<ConstantDataArray>(Initializer);
	if (!CDA->isString())
	  continue;

	// Extract raw string
	StringRef StrVal = CDA->getAsString();	
	const char *Data = StrVal.begin();
	const int Size = StrVal.size();
	
	// Create encoded string variable. Constants are immutable so we must override with a new Constant.
	char *NewData = EncodeString(Data, Size);
	Constant *NewConst = ConstantDataArray::getString(Ctx, StringRef(NewData, Size), false);

	// Overwrite the global value
	Glob.setInitializer(NewConst);

	GlobalStrings.push_back(new GlobalString(&Glob, Size));
	Glob.setConstant(false);
      }
      else if(isa<ConstantStruct>(Initializer))
      {
	// Handle structs
	auto CS = cast<ConstantStruct>(Initializer);
	if(Initializer->getNumOperands() > 1)
	  continue; // TODO: Fix bug when removing this: "Constant not found in constant table!"
	for(unsigned int i = 0; i < Initializer->getNumOperands(); i++)
	{
	  auto CDA = dyn_cast<ConstantDataArray>(CS->getOperand(i));
	  if (!CDA || !CDA->isString())
	    continue;

	  // Extract raw string
	  StringRef StrVal = CDA->getAsString();
	  const char *Data = StrVal.begin();
	  const int Size = StrVal.size();

	  // Create encoded string variable
	  char *NewData = EncodeString(Data, Size);
	  Constant *NewConst = ConstantDataArray::getString(Ctx, StringRef(NewData, Size), false);

	  // Overwrite the struct member
	  CS->setOperand(i, NewConst);

	  GlobalStrings.push_back(new GlobalString(&Glob, i, Size));
	  Glob.setConstant(false);
	}
      }
    }

    return GlobalStrings;
  }

  struct StringObfuscatorModPass : public PassInfoMixin<StringObfuscatorModPass>
  {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
    {
      // Transform the strings
      auto GlobalStrings = encodeGlobalStrings(M);
      
      Function* DecodeFunc = createDecodeFunc(M);
      Function* DecodeStub = createDecodeStubFunc(M, GlobalStrings, DecodeFunc);

      // Inject a call to DecodeStub from main
      Function *MainFunc = M.getFunction("main");
      if (MainFunc)
	createDecodeStubBlock(MainFunc, DecodeStub);
      
      return PreservedAnalyses::all();
    };
  };
} // end anonymous namespace

llvm::PassPluginLibraryInfo
getPassPluginInfo()
{
  const auto callback = [](PassBuilder &PB)
  {
    PB.registerPipelineEarlySimplificationEPCallback(
	[&PB](llvm::ModulePassManager &MPM, auto... other_args)
	{
          MPM.addPass(StringObfuscatorModPass());
	  outs() << "[INFO] Loaded StringObfuscation pass.\n";
          return true;
        });
  };

  return { LLVM_PLUGIN_API_VERSION,
	   "StringObfuscatorPass",
	   LLVM_VERSION_STRING,
	   callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK
llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return getPassPluginInfo();
}
