pipeline {
    agent any
    stages {
        stage ('Build') {
            steps {
                sh 'echo "Building..."'
                sh 'chmod +x ./scripts/Linux-Build.sh'
                sh './scripts/Linux-Build.sh'
                archiveArtifacts artifacts: 'Release/tests/YAML_Lib_Unit_Tests, Release/tests/YAML_Lib_Fuzz_Tests, Release/libYAML_Lib.a, Release/examples/YAML_*', fingerprint: true
            }
        }
        stage ('Style & Static Analysis') {
            steps {
                sh 'echo "Running code style and static analysis checks..."'
                sh 'chmod +x ./scripts/Linux-Style-Check.sh'
                sh './scripts/Linux-Style-Check.sh'
            }
        }
        stage ('Test'){
            steps {
                sh 'echo "Testing..."'
                sh 'chmod +x ./scripts/Linux-Run-Tests.sh'
                sh './scripts/Linux-Run-Tests.sh'
            }
        }
        stage ('Sanitizer Builds') {
            steps {
                sh 'echo "Running sanitizer builds..."'
                sh 'chmod +x ./scripts/Linux-Build-Sanitizers.sh'
                sh './scripts/Linux-Build-Sanitizers.sh'
            }
        }
        stage ('Minimal Safe Build') {
            steps {
                sh 'echo "Building minimal safe library..."'
                sh 'chmod +x ./scripts/Linux-Build-Minimal.sh'
                sh './scripts/Linux-Build-Minimal.sh'
            }
        }
    }
}