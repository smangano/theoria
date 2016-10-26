

//Builds a chain of resolvers. Used by Config Builder
class ConfigResolverBuilder : public core::Component 
{
public:

    ConfigResolverBuilder() {}

    /* Reads list of <theoria::config::ConfigVariableResolver>'s to determine the 
     * resolver list returned by <build>
     * @inherit
     */
	virtual Dependencies init(const Config& config) overide;
   
    virtual void finalize(ComponentList dependencies) ;

    config::ConfigVariableResolver* build() const {return _resolvers;}

private:

    config::ConfigVariableResolver * _resolvers ;
    
} ;
