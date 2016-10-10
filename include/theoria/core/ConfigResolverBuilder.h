

//Builds a chain of reslovers. Used by Config Builder
class ConfigResolverBuilder : public core::Component 
{
public:

    ConfigResolverBuilder() {}

    /* Reads list of <theoria::config::ConfigVariableResolver>'s to determin the 
     * resolver list returned by <build>
     * @inherit
     */
	virtual Dependencies init(const Config& config) overide;
   
    virtual void finalize(ComponentList dependencies) ;

    config::ConfigVariableResolver* build() const {return _resolvers;}

private:

    config::ConfigVariableResolver * _resolvers ;
    
} ;
