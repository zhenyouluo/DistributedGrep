#include "grep_diy.h"

//prepare filename's RegExp
string filename_format( string ori ){

	string filename_regex = ori;
	if(filename_regex.c_str()[0] == '*'){
		filename_regex.assign( filename_regex.c_str() + 1 );
		filename_regex += "$";
	}

	return filename_regex;

}

int grep_file(ifstream * iffile, ofstream * offile, string filename, regex_t * regex, string option){
	(*iffile).open(filename.c_str());
	if(!iffile->is_open()){
		cout<<"cannot open "<<filename<<endl;
		return 1;
	}

	int linenum = 0;
	string line;
	while(!iffile->eof()){
		linenum++;
		getline((*iffile), line);
		cout<<line<<endl;
		if(regexec( regex, line.c_str(), (size_t) 0, NULL, 0 ) == 0){
        	(*offile)<<line<<endl;
        	cout<<line<<endl;
		}
	}

	return 0;
}

int grep_diy( string output, string cmd ){
	
	//clear the output file
	ofstream out_f;
	out_f.open( output.c_str() );
	if( !out_f.is_open() ){
		cout<<"cannot open "<<output<<endl;
		return 1;
	}

	//split command by blackspace
	string buf;
	vector<string> cmds;
	stringstream ss(cmd);
	while( ss >> buf )
		cmds.push_back(buf);

	//get the filename restriction, change it to RegExp
	string filename_regex = filename_format( cmds[cmds.size()-1] );

	//prepare the regex_t for filename filtering
    regex_t reg_c;
    if (regcomp( &reg_c, filename_regex.c_str(), REG_EXTENDED|REG_NOSUB) !=0){
    	cout<<filename_regex<<" is not a valid RegExp"<<endl;
    	return 1;
    }

	//query files' names in current work directry
	char * cwd;
	cwd = getcwd(NULL, 0);
	struct dirent *dp;
    DIR *dirp = opendir( cwd );
    free(cwd);

    //find files matching filename_regex
    vector<string> files;
    while ( (dp=readdir(dirp)) != NULL ){
        if(regexec( &reg_c, dp-> d_name, (size_t) 0, NULL, 0 ) == 0)
        	files.push_back( dp->d_name );
    }
    closedir(dirp);

    //extract the RegExp from cmds
    string grep_regex = cmds[1];
    string option = " ";
    if( grep_regex.c_str[0] == '-' ){
    	option = grep_regex;
    	grep_regex = cmds[2];
    	
    	for(int i=3; i < cmds.size()-1; i++){
    		grep_regex += " ";
    		grep_regex += cmds[i];
    	}
    }
    else{
    	for(int i=2; i < cmds.size()-1; i++){
    		grep_regex += " ";
    		grep_regex += cmds[i];
    	}
    }

    //prepare the regex_t for line grepping
    if (regcomp( &reg_c, grep_regex.c_str(), REG_EXTENDED|REG_NOSUB) !=0){
    	cout<<grep_regex<<" is not a valid RegExp"<<endl;
    	return 1;
    }

    ifstream in_f;
    for(int i=0; i < files.size(); i++){
    	grep_file( &in_f, &out_f, files[i], &reg_c, option );
    }

    cmds.clear();
    files.clear();
    regfree( &reg_c );
    out_f.close();

    return 0;
}