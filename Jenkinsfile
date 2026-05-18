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
    }
}