# Introduction
Almost all programs are configurable. Even simple commad line utilities take options to twaek their 
behavior. Windows programs use the Windows's Registry. Unix programs use
files like .bashrc .vimrc, etc. As software grows in complexity,
configuration requirements create challenges. In an enterprise, software needs to run in
different environments (DEV, UAT, QA, PROD), different regions (AMRS,
EMEA, ASIA). There are settings related to locales, languages, networks,
security, performance tweaks and so on. 

# Theoria Configuration
Theoria programs use TOML files for configuration by default. This format is highly regarded for it expressivenes and
readability. However, Theoria also allows you to use alternative formats like XML and YAML. You can even roll your own. 
Regardless of format, Theoria configuration supports DRY principles via a powerfull paramterization mechanism that hooks
filebased configuration into other configuration sources like command line paramters, OS environment variables,
databases, web services, etc. No matter what standards your company uses for configuration, you can easily adapt Theoria
to it. Further, you can use simple schemes in DEV and more enterprise conforming ones in UAT and PROD. 

# Theoria Configures Components
A Theoria application is components based. This means Theoria focus configuration at the component level. Theoria
configuration files offer a window into each application's behavior. This means non-developers can read and modify configuratipn
files with out coding skills. 

## A Sample TOML Configuration

  
 
 
