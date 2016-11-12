

/* @Deprecated
 * Builds a chain of resolvers. Used by Config Builder
 */
class ConfigResolverBuilder : public core::Component 
{
public:

    ConfigResolverBuilder() {}

    /* Reads list of <theoria::config::ConfigVariableResolver>'s to determine the 
     * resolver list returned by <build>
     * @inherit
     */
	Dependencies init(const Config& config) overide;
   
    /**
     * Receives a ConfigBuilder  and one or more resolvers and wires the resolvers as 
     * the builders resolver chain in the order they appear
     */
    void finalize(ComponentList dependencies) override ;
    config::ConfigVariableResolver* build() const {return _resolvers;}

private:

    config::ConfigVariableResolver * _resolvers ;
    
} ;
