<font size="20"> Theoria - A C++ Application Container for the Real and Virtual World </font>

#NOTE: Theoria is in pre-release state. This document will be expanded when release 0.1 is ready


# Theoria

Theoria is a C++ Framework for building highly configurable applications that can be monitored in Virtual Reality

## Vision
Imagine you can explore a running program as a Virtual World. Imagine you can dive into the layers of complexity as if
you were naviaging a city or a complex building with familar visual cues. Imagine if the most relevant information explaining the state of an application was there to see rather than being buried in log files. Imagine being able to visualize the organization of an application - how its components connect and also see this interaction as it happens. This is the vision of Theoria and it is only the beginning.

## Introduction
Theoria allows you to build highly configurable applications composed of interacting components. These components can interact synchronously via direct references or asynchronously via message passing. I wide variety of threading models are provided and you can create your own. Theoria uses TOML for configuration but you can customize the configuration manager if you prefer JSON, YAML, XML or other formats.

Theoria was designed using a few basic principles:

1) Zero Cost Abstraction - don't make the user pay for what she does not use
2) Transparency - the current state, configuration and organization of an application should be immedialy availibe for inspection
3) Configurability - complex applications need a lot of configuration information so make configuration flexible and powerful.

You can learn more about Theoria's features and how they are delivered as zero cost abstractions in the sections below.  

## The Theoria Application Container
Theoria is both a library and an application container. When you build theoria you get an executable named __theoria__ 
The Theoria executable acts a a host for your application and provides many services to your application. However,
the application container is optional. You do not have to use it if you prefer to role your own. But you should at least
take some time to undertsand what it does before dismissing it out right!

A theoria application is run as follows:

#### theoria _yourConfigFile_ --option1 --option2 ... &nbsp;&nbsp; __--__ &nbsp;&nbsp;  _yourvar1=1_ _yourvar2=2_ ...
    
    Item             | Description
    ---------------- | -------------------------------------------------------------------------------
    yourConfigFile | a file (usually in TOML format) that defines your components and their settings
    --option         | options that alter the behavior of theoria in some way (e.g., theoria --help)
    --               | a seperator betwen theoria's options and your applications config variables
    var=val          | configuration variables specific to your application. These can be used in the config file or in code
                       

