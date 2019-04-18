import qbs
import qbs.TextFile
import qbs.File

Module {
    Rule {
        condition: project.isWindows
        inputs: "RC_TMPL"
        multiplex: false
        alwaysRun: true
        Artifact {
            filePath: product.name+"_generated.rc"
            fileTags: "rc"
        }
        prepare: {
            var  cmd  =  new  JavaScriptCommand();
            cmd.description  =  "Processing  '"  +  input.fileName  +  "'";
            cmd.highlight  =  "codegen";
            cmd.sourceCode  =  function()  {
                var  file  =  new  TextFile(input.filePath);
                var  content  =  file.readAll();
                file.close();

                content  =  content.replace(/%MainIconPath%/g, product.sourceDirectory+"/MainIcon.ico");
                content  =  content.replace(/%FILEVERSION%/g, project.softwareVersion.replace(/\./g,","));
                content  =  content.replace(/%PRODUCTVERSION%/g, project.softwareVersion.replace(/\./g,","));
                content  =  content.replace(/%FILEDESCRIPTION%/g, product.description);
                content  =  content.replace(/%FILEVERSION_STR%/g, project.softwareVersion);
                content  =  content.replace(/%PRODUCTVERSION_STR%/g, project.softwareVersion);
                content  =  content.replace(/%COMPANY_STR%/g, project.company);
                content  =  content.replace(/%LEGALCOPYRIGHT_STR%/g, "Copyright (C) "+ (new (Date)).getFullYear()+", "+project.company);
                content  =  content.replace(/%ORIGINALFILENAME_STR%/g, product.name+".exe");
                content  =  content.replace(/%PRODUCTNAME_STR%/g, project.productName);
                content  =  content.replace(/%INTERNALNAME_STR%/g, product.internalName);

                file  =  new  TextFile(output.filePath,  TextFile.WriteOnly);
                file.truncate();
                file.write(content);
                file.close();
            }
            return  cmd;
        }
    }
}
