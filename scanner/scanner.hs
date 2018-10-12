module Scanner (
    ScannerError(..),
    scan,
    step
) where

import Data.Char (isAlpha, isDigit, isSpace)
import Control.Monad.Except (throwError)

data TokenData = 
    Ident String    -- not more than 10 characters, truncate if exceed
    | Number Int
    ------------------
    | Add
    | Sub
    | Mul
    | Div
    | Mod
    ------------------
    | Comma
    | Dot
    | Semicolon
    ------------------
    | OpenParenthesis       -- (
    | ClosedParenthesis     -- )
    | OpenSquareBracket     -- [
    | ClosedSquareBracket   -- ]
    ------------------
    | LT    -- <
    | GT    -- >
    | EQ    -- =
    ------------------
    | GE    -- >=
    | LE    -- <=
    | NE    -- <>
    | Assign -- :=
    ------------------
    -- Total of 16 keywords
    | Begin
    | Call
    | Const
    | Do
    | End
    | Else
    | For
    | If
    | Odd
    | Procedure
    | Program
    | Function
    | Then
    | To
    | Var
    | While
    deriving (Show, Eq)

data Token = Token {
    tokenData :: TokenData,
    tokenIndex :: Int
} deriving (Show, Eq)

data ScannerError = 
    NotRecognizeToken
    deriving (Show, Eq)

data FAState =
    FAStart
    | FANumber String
    | FAName String
    | FASingleSpecial Char
    | FAMultiSpecial String
    deriving (Show, Eq)

data StepResult = 
    StepNoResult FAState
    | StepResult (Either ScannerError Token)
    deriving (Show, Eq)

------------------------------------------
isUnderscore :: Char -> Bool
isUnderscore '_' = True
isUnderscore _ = False
------------------------------------------

------------------------------------------
isSingleSpecial :: Char -> Bool

isSingleSpecial '+' = True
isSingleSpecial '-' = True
isSingleSpecial '*' = True
isSingleSpecial '/' = True
isSingleSpecial '%' = True

isSingleSpecial ',' = True
isSingleSpecial '.' = True
isSingleSpecial ';' = True

isSingleSpecial '=' = True

isSingleSpecial '(' = True
isSingleSpecial ')' = True
isSingleSpecial '[' = True
isSingleSpecial ']' = True

isSingleSpecial _ = False
------------------------------------------

------------------------------------------
isMultiSpecial :: Char -> Bool

isMultiSpecial ':' = True
isMultiSpecial '<' = True
isMultiSpecial '>' = True
isMultiSpecial '=' = True

isMultiSpecial _ = False
------------------------------------------

------------------------------------------
step :: FAState -> Char -> Int -> StepResult

step FAStart char index
    | (isAlpha char) || (isUnderscore char) = StepNoResult $ FAName [char]
    | isDigit char = StepNoResult $ FANumber [char]
    | isSingleSpecial char = StepNoResult $ FASingleSpecial char
    | isMultiSpecial char = StepNoResult $ FAMultiSpecial [char]
    | (isSpace char) || char == '\0' = StepNoResult $ FAStart
    | otherwise = StepResult $ throwError NotRecognizeToken

step (FAName name) char index 
    | (isAlpha char) || (isUnderscore char) || (isDigit char) = 
            StepNoResult $ FAName (name ++ [char])
    | otherwise = StepResult $ return $ Token (Ident name) index
------------------------------------------

------------------------------------------
multiStep :: FAState -> String -> Int -> Either ScannerError [Token]
multiStep _ [] _ = return []
multiStep state (x:xs) index = 
    let result = step state x index in
    case result of 
        (StepNoResult newState) -> multiStep newState xs (index + 1)
        (StepResult eitherToken) -> do
            token <- eitherToken
            remains <- multiStep FAStart (x:xs) index
            return (token : remains)
------------------------------------------

scan :: String -> Either ScannerError [Token]
scan s = multiStep FAStart (s ++ ['\0']) 0
