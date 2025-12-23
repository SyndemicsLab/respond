# Why RESPOND?

The RESPOND model was first developed in 2018 as a response to the rapid rise in opioid use disorder (OUD) cases across the country. Funded by the National Institute on Drug Abuse (NIDA), RESPOND was developed to study care options for OUD and various cost effectiveness measurements to inform system-level change and provide evidence to policy makers to make informed options for the community.

## Costing and Utility

A core outcome of interest for the RESPOND model is the cost effectiveness of policy alternatives. Because modeling lets us test alternatives to the current policy, the natural questions that always follow a proposed policy change are:

1. How much money does it cost?
2. How many people would it help?

Answering these questions is the goal of cost effectiveness modeling. Noticably, cost effectiveness is not about spending the least amount of money, but rather spending the entire allocated amount of money in a way that effectively helps the most amount of people.

This is a particularly difficult challenge for cohort based models as such models do not model individual people. Instead, cohort models replicate changes in the population. This means that cost effectiveness must be considered on a population level. Given the Markovian nature of the model, we can apply our cost metrics to the final state of the model. So, we are able to completely remove cost effectiveness from the model runtime and apply it as a post processing procedure when indicated by the person running the analysis.

## Why Refactor?

This new edition of RESPOND (informally called v2) was written to help maintainability of the model and enable faster development and extension on the base product. Written initially in R, RESPOND was proving very difficult to extend to new studies. The graph was a rigidly developed structure that required code edits to incorporate new treatment and behavior options, which rapidly proved a major bottleneck with the rise of fentanyl, drug adulteration and stimulant mixing, and changes in confounding health conditions. Thus, we re-designed the model to be a state transition model with flexible dimensions to expand as needed.

Part of the refactor was transitioning away from the R programming language. While we recognize various benefits of the R language, we felt we wanted more control over the model and wanted to natively operate in C++ so that we could build out packages for various languages rather than be confined to R for all future development. We also believe that the object oriented nature of the model was better represented in languages like C++.

Previous: [Home](index.md)

Next: [Installation](installation.md)
