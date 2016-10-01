
if __name__ == "__main__":
    parser = OptionParser()
    Runtime.add_options(parser)
    options, args = parser.parse_args()
    pid, players = load_config("{{VIFF_CONFIG_LOC}}")
    Zp = GF(find_prime(2**65, blum=True))
    
    runtime_class = make_runtime_class(
        mixins=[ProbabilisticEqualityMixin, ComparisonToft07Mixin]
    )
    pre_runtime = create_runtime(pid, players, 1, options, 
        runtime_class=runtime_class)
    pre_runtime.addCallback(protocol, Zp)
    pre_runtime.addErrback(report_error)

    reactor.run()
