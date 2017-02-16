#include "FileProcessorAdapter.h"
#include "flowline/Context.h"
#include "flowline/Resource.h"
#include "fileadapter/FileResource.h"
#include "base/ClassRegister.h"


using namespace flowline;
namespace fileadapter
{

REGISTER_CLASS("FileCopyAdapter", FileProcessorAdapter,Adapter);

FileProcessorAdapter::FileProcessorAdapter()
{
}

FileProcessorAdapter::~FileProcessorAdapter()
{
}

void FileProcessorAdapter::process( Context & ctx, PResource input, vector<PResource>& output ){
	    cout << "ProcessorAdapter::process()" <<endl;
	    cout << "ProcessorAdapter::input.get()" << input.get() <<endl;  
	    FileResource* finput = dynamic_cast< FileResource* >(input.get());
        // string inputfile = finput->getName();
        // string indir = finput->getDirectory();
        cout << "ProcessorAdapter::process()copy file" <<endl;
        PResource res = createResource( ctx );
        cout << "ProcessorAdapter::res.get()" << res.get() <<endl;      
        FileResource* ftrg = dynamic_cast< FileResource* >(res.get());
        //Yuaneg 2005-9-5 把push_back函数提前,防止copyTo异常时不执行
        output.push_back( res );
        finput->copyTo(*ftrg,ctx);
        // string outputfile = ftrg->getTempName();
        // string outdir = ftrg->getDirectory();
        // File  in(indir,inputfile);
        // File  out(outdir,outputfile);
           
        // in.getAbsolutePath(inputfile);
        // out.getAbsolutePath(outputfile);
        // string sh = "cp '";        
        // sh = sh + inputfile + "'  '" + outputfile + "'";
        // cout << "shell: " << sh << endl;  
                       
        //PData d( new FileData( "output data" ) );
        //res->write( ctx, d );
        //system(sh.c_str());        
       //output.push_back( res );
}
void FileProcessorAdapter::finish( Context& ctx, vector<PResource> & output ) {
	 	cout << "ProcessorAdapter::finish()"  << __LINE__  << endl;
}


};
